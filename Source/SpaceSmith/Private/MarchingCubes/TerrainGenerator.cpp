// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGenerator.h"
#include "MarchingCubes/TerrainWorker.h"
#include <../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h>
#include "MarchingCubes/TerrainChunk.h"
#include "MarchingCubes/TerrainData.h"
#include "../Plugins/SimplexNoise/Source/SimplexNoise/Public/SimplexNoiseBPLibrary.h"
#include "SpaceSmithCharacter.h"
#include "Miscellaneous/UtilityTimer.h"
#include "MarchingCubes/TerrainChunkWorker.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATerrainGenerator::ATerrainGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATerrainGenerator::BeginPlay()
{
	Super::BeginPlay();

	TerrainWorker = new FTerrainWorker();
	TerrainWorker->Start();

	ChunkWorker = new FTerrainChunkWorker();
	ChunkWorker->Start();

	Grid = new UTerrainData();

	Character = Cast<ASpaceSmithCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

// Called every frame
void ATerrainGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckPlayerPosition();
	GenerateChunk();
	GenerateChunkMesh();
	UpdateTerrain();

	if (bDebug)
	{
		for (auto & ChunkPosition : CalculatingChunks)
		{
			DrawDebugBox(GetWorld(), ChunkToWorld(ChunkPosition), (FVector(ChunkScale) * FVector(ChunkSize) / 2.0f), FColor::Yellow, false, -1, 0, 50.0f);
		}

		for (const TPair<FIntVector, UTerrainChunk*>& Chunk : Chunks)
		{
			if (Chunk.Value->HasChanges())
				DrawDebugBox(GetWorld(), ChunkToWorld(Chunk.Key), (FVector(ChunkScale) * FVector(ChunkSize) / 2.0f), FColor::Green, false, -1, 0, 50.0f);
		}
	}
}

void ATerrainGenerator::CheckPlayerPosition()
{
	if (!Character)
	{
		return;
	}

	FIntVector CharacterChunkLocation = WorldToChunk(Character->GetActorLocation());

	if (LastCharacterChunkLocation == CharacterChunkLocation)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Spawn Chunks")));
	TArray<FIntVector> ChunkLocations;

	for (int32 X = CharacterChunkLocation.X - NumSpawnChunk.X; X <= CharacterChunkLocation.X + NumSpawnChunk.X; X++)
	{
		for (int32 Y = CharacterChunkLocation.Y - NumSpawnChunk.Y; Y <= CharacterChunkLocation.Y + NumSpawnChunk.Y; Y++)
		{
			for (int32 Z = CharacterChunkLocation.Z - NumSpawnChunk.Z; Z <= CharacterChunkLocation.Z + NumSpawnChunk.Z; Z++)
			{
				FIntVector ChunkLocation = FIntVector(X, Y, Z);
				ChunkLocations.Add(ChunkLocation);
			}
		}
	}
	ChunkLocations.Sort([&](const FIntVector& A, const FIntVector& B)
	{
		float DistanceA = (CharacterChunkLocation - A).Size();
		float DistanceB = (CharacterChunkLocation - B).Size();
		return DistanceA < DistanceB;
	});

	LastCharacterChunkLocation = CharacterChunkLocation;

	for (auto & ChunkLocation : ChunkLocations)
	{
		CreateChunk(ChunkLocation);
	}
}

void ATerrainGenerator::GenerateChunk()
{
	if (!ChunkWorker)
	{
		return;
	}

	if (ChunkWorker->FinishedWorks.Num() == 0)
	{
		return;
	}

	FTerrainChunkWorkerInformation FinishedWork;
	if (ChunkWorker->Dequeue(FinishedWork))
	{
		FString ComponentName;
		int32 ID = Chunks.Num();
		ComponentName.Append(TEXT("Chunk"));
		ComponentName.AppendInt(ID);
		FName ChunkComponentName;
		ChunkComponentName.AppendString(ComponentName);
		UTerrainChunk* NewChunk = NewObject<UTerrainChunk>(this, ChunkComponentName);
		NewChunk->RegisterComponent();

		if (TerrainMaterial)
		{
			NewChunk->SetMaterial(0, TerrainMaterial);
		}
		else
		{
			NewChunk->SetMaterial(0, UMaterial::GetDefaultMaterial(MD_Surface));
		}

		NewChunk->Generator = this;
		NewChunk->ChunkLocation = FinishedWork.ChunkLocation;
		NewChunk->SetChanges(true);
		Chunks.Add(FinishedWork.ChunkLocation, NewChunk);

		for (auto & ChunkLocation : FinishedWork.EdgeChunks) 
		{
			if (UTerrainChunk* Chunk = GetChunk(ChunkLocation))
			{
				Chunk->SetChanges(true);
			}
		}
		CalculatingChunks.Remove(FinishedWork.ChunkLocation);
	}
}

void ATerrainGenerator::GenerateChunkMesh()
{
	if (!TerrainWorker)
	{
		return;
	}

	if (TerrainWorker->FinishedWorks.Num() == 0)
	{
		return;
	}

	FTerrainWorkerInformation FinishedWork;
	if (TerrainWorker->Dequeue(FinishedWork))
	{
		UTerrainChunk** FoundChunk = Chunks.Find(FinishedWork.ChunkLocation);
		if (!FoundChunk)
		{
			return;
		}
		UTerrainChunk *TerrainChunk = *FoundChunk;
		TerrainChunk->bUpdating = false;

		if (!FinishedWork.bValid)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("This Chunk has Changes, Skip and Regenerate")));
			UpdateChunk(TerrainChunk->ChunkLocation);
			return;
		}

		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Chunk(%s) Dequeue with %d Vertices"), *FinishedWork.ChunkLocation.ToString(), FinishedWork.Vertices.Num()));
		TArray<FVector2D> UVs;
		TArray<FColor> VertexColors;

		TerrainChunk->ClearAllMeshSections();
		TerrainChunk->CreateMeshSection_LinearColor(0, FinishedWork.Vertices, FinishedWork.Indices, FinishedWork.Normals, FinishedWork.UVs, FinishedWork.VertexColors, FinishedWork.Tangents, true);
		if (TerrainChunk->HasChanges())
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Regenerate Mesh, Outdated"), FinishedWork.Vertices.Num()));
			UpdateChunk(TerrainChunk->ChunkLocation);
		}
	}
}

void ATerrainGenerator::BeginDestroy()
{
	if (TerrainWorker)
	{
		TerrainWorker->EnsureCompletion();
		TerrainWorker->Shutdown();
		delete TerrainWorker;
		TerrainWorker = nullptr;
	}

	if (ChunkWorker)
	{
		ChunkWorker->EnsureCompletion();
		ChunkWorker->Shutdown();
		delete ChunkWorker;
		ChunkWorker = nullptr;
	}

	Super::BeginDestroy();
}

bool ATerrainGenerator::CreateChunk(FIntVector ChunkLocation)
{
	if (Chunks.Contains(ChunkLocation))
	{
		// 청크가 그 위치에 이미 있으면 만들지 않는다
		return false;
	}

	FTerrainChunkWorkerInformation Information;
	Information.ChunkLocation = ChunkLocation;
	Information.ChunkSize = ChunkSize;
	Information.Grid = Grid;
	Information.Generator = this;

	ChunkWorker->Enqueue(Information);
	CalculatingChunks.Add(ChunkLocation);
	return true;
}

UTerrainChunk* ATerrainGenerator::GetChunk(FIntVector ChunkLocation)
{
	UTerrainChunk **FoundChunk = Chunks.Find(ChunkLocation);
	if (FoundChunk)
	{
		return *FoundChunk;
	}

	return nullptr;
}

bool ATerrainGenerator::UpdateChunk(FIntVector ChunkLocation)
{
	UTerrainChunk** FoundChunk = Chunks.Find(ChunkLocation);
	if (!FoundChunk)
	{
		return false;
	}

	UTerrainChunk *TerrainChunk = *FoundChunk;
	if (TerrainChunk->bUpdating)
	{
		// 청크가 이미 업데이트 중이면 하지 않고, 다음에 업데이트하도록 표시한다
		TerrainChunk->SetChanges(true);
		return false;
	}


	// 이웃 청크가 업데이트중이라면 하지 않는다.
	for (int32 X = -1; X <= 1; X++)
	{
		for (int32 Y = -1; Y <= 1; Y++)
		{
			for (int32 Z = -1; Z <= 1; Z++)
			{
				if(X == 0 && Y == 0 && Z == 0)
					continue;

				FIntVector NeighborChunk = ChunkLocation + FIntVector(X, Y, Z);

				if (CalculatingChunks.Contains(NeighborChunk))
				{
					return false;
				}
			}
		}
	}
	TerrainChunk->bUpdating = true;
	TerrainChunk->SetChanges(false);

	FTerrainWorkerInformation Information;
	Information.Grid = Grid;
	Information.Chunk = TerrainChunk;
	Information.ChunkLocation = ChunkLocation;
	Information.ChunkScale = ChunkScale;
	Information.ChunkSize = ChunkSize;
	Information.IsoValue = IsoValue;
	Information.Generator = this;

	TerrainWorker->Enqueue(Information);
	return true;
}

bool ATerrainGenerator::DestroyChunk(FIntVector ChunkLocation)
{
	UTerrainChunk **FoundChunk = Chunks.Find(ChunkLocation);
	if (!FoundChunk)
	{
		return false;
	}

	UTerrainChunk *TerrainChunk = *FoundChunk;
	TerrainChunk->UnregisterComponent();
	TerrainChunk->DestroyComponent();
	Chunks.Remove(ChunkLocation);

	return true;
}

bool ATerrainGenerator::SetVoxel(FIntVector GridLocation, float Value, bool bLateUpdate)
{
	Grid->SetVoxelDensity(GridLocation, Value);

	int32 ChunkX = FMath::FloorToInt((float)GridLocation.X / ChunkSize.X);
	int32 ChunkY = FMath::FloorToInt((float)GridLocation.Y / ChunkSize.Y);
	int32 ChunkZ = FMath::FloorToInt((float)GridLocation.Z / ChunkSize.Z);

	UTerrainChunk* TerrainChunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
	if (!TerrainChunk)
	{
		CreateChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
		return false;
	}

	if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f)
	{
		if (bLateUpdate)
		{
			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX - 1, ChunkY, ChunkZ)))
			{
				Chunk->SetChanges(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ)))
			{
				Chunk->SetChanges(true);
			}
		}
		else
		{
			UpdateChunk(FIntVector(ChunkX - 1, ChunkY, ChunkZ));
			UpdateChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ));
		}
	}

	if (FMath::Fmod(GridLocation.Y, ChunkSize.Y) == 0.0f)
	{
		if (bLateUpdate)
		{
			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY - 1, ChunkZ)))
			{
				Chunk->SetChanges(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ)))
			{
				Chunk->SetChanges(true);
			}
		}
		else
		{
			UpdateChunk(FIntVector(ChunkX, ChunkY - 1, ChunkZ));
			UpdateChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ));
		}
	}

	if (FMath::Fmod(GridLocation.Z, ChunkSize.Z) == 0.0f)
	{
		if (bLateUpdate)
		{
			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ + 1)))
			{
				Chunk->SetChanges(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1)))
			{
				Chunk->SetChanges(true);
			}
		}
		else
		{
			UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ + 1));
			UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1));
		}
	}

	if (bLateUpdate)
	{
		TerrainChunk->SetChanges(true);
	}
	else
	{
		UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
	}
	return true;
}

void ATerrainGenerator::SetVoxels(const TArray<FIntVector>& GridLocations, float Value)
{
	for (auto & GridLocation : GridLocations)
	{
		SetVoxel(GridLocation, Value, true);
	}
}

bool ATerrainGenerator::AddVoxel(FIntVector GridLocation, float Value, bool bLateUpdate /*= false*/)
{
	Grid->AddVoxelDensity(GridLocation, Value);

	int32 ChunkX = FMath::FloorToInt((float)GridLocation.X / ChunkSize.X);
	int32 ChunkY = FMath::FloorToInt((float)GridLocation.Y / ChunkSize.Y);
	int32 ChunkZ = FMath::FloorToInt((float)GridLocation.Z / ChunkSize.Z);

	UTerrainChunk* TerrainChunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
	if (!TerrainChunk)
	{
		CreateChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
		return false;
	}

	if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f)
	{
		if (bLateUpdate)
		{
			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX - 1, ChunkY, ChunkZ)))
			{
				Chunk->SetChanges(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ)))
			{
				Chunk->SetChanges(true);
			}
		}
		else
		{
			UpdateChunk(FIntVector(ChunkX - 1, ChunkY, ChunkZ));
			UpdateChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ));
		}
	}

	if (FMath::Fmod(GridLocation.Y, ChunkSize.Y) == 0.0f)
	{
		if (bLateUpdate)
		{
			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY - 1, ChunkZ)))
			{
				Chunk->SetChanges(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ)))
			{
				Chunk->SetChanges(true);
			}
		}
		else
		{
			UpdateChunk(FIntVector(ChunkX, ChunkY - 1, ChunkZ));
			UpdateChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ));
		}
	}

	if (FMath::Fmod(GridLocation.Z, ChunkSize.Z) == 0.0f)
	{
		if (bLateUpdate)
		{
			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ + 1)))
			{
				Chunk->SetChanges(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1)))
			{
				Chunk->SetChanges(true);
			}
		}
		else
		{
			UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ + 1));
			UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1));
		}
	}

	if (bLateUpdate)
	{
		TerrainChunk->SetChanges(true);
	}
	else
	{
		UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
	}
	return true;
}

void ATerrainGenerator::AddVoxels(const TArray<FIntVector>& GridLocations, float Value)
{
	for (auto & GridLocation : GridLocations)
	{
		AddVoxel(GridLocation, Value, true);
	}
}

float ATerrainGenerator::GetVoxel(FIntVector GridLocation)
{
	if (Grid)
		return Grid->GetVoxelDensity(GridLocation);

	return 0;
}

void ATerrainGenerator::UpdateTerrain()
{
	for (const TPair<FIntVector, UTerrainChunk*> Chunk : Chunks)
	{
		if (Chunk.Value->HasChanges())
		{
			UpdateChunk(Chunk.Key);
		}
	}
}

FIntVector ATerrainGenerator::WorldToGrid(FVector WorldLocation)
{
	int32 X = FMath::FloorToInt(WorldLocation.X / ChunkScale.X);
	int32 Y = FMath::FloorToInt(WorldLocation.Y / ChunkScale.Y);
	int32 Z = FMath::FloorToInt(WorldLocation.Z / ChunkScale.Z);

	return FIntVector(X, Y, Z);
}

FIntVector ATerrainGenerator::WorldToChunk(FVector WorldLocation)
{
	int32 GridX = FMath::FloorToInt(WorldLocation.X / ChunkScale.X);
	int32 GridY = FMath::FloorToInt(WorldLocation.Y / ChunkScale.Y);
	int32 GridZ = FMath::FloorToInt(WorldLocation.Z / ChunkScale.Z);

	int32 ChunkX = FMath::FloorToInt((float)GridX / ChunkSize.X);
	int32 ChunkY = FMath::FloorToInt((float)GridY / ChunkSize.Y);
	int32 ChunkZ = FMath::FloorToInt((float)GridZ / ChunkSize.Z);

	return FIntVector(ChunkX, ChunkY, ChunkZ);
}

FVector ATerrainGenerator::ChunkToWorld(FIntVector ChunkLocation)
{
	FVector WorldLocation;
	WorldLocation.X = (ChunkLocation.X + 0.5f) * ChunkScale.X * ChunkSize.X;
	WorldLocation.Y = (ChunkLocation.Y + 0.5f) * ChunkScale.Y * ChunkSize.Y;
	WorldLocation.Z = (ChunkLocation.Z + 0.5f) * ChunkScale.Z * ChunkSize.Z;
	return WorldLocation;
}

FORCEINLINE FIntVector ATerrainGenerator::GetPlayerChunkPosition() const
{
	FIntVector ChunkPosition = LastCharacterChunkLocation;
	return ChunkPosition;
}
