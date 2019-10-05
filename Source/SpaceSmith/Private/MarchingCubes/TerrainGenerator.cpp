// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGenerator.h"
#include "MarchingCubes/TerrainWorker.h"
#include <../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h>
#include "MarchingCubes/TerrainChunk.h"
#include "MarchingCubes/TerrainData.h"
#include "SpaceSmithCharacter.h"
#include "Miscellaneous/UtilityTimer.h"
#include "MarchingCubes/TerrainChunkWorker.h"
#include "DrawDebugHelpers.h"

DECLARE_CYCLE_STAT(TEXT("Terrain Generator ~ CopyVoxelData To TerrainWorker"), STAT_CopyVoxelDataToTerrainWorker, STATGROUP_TerrainWorker);

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

	ChunkWorker = new FTerrainChunkWorker(this);
	ChunkWorker->Start();

	Character = Cast<ASpaceSmithCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void ATerrainGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ATerrainGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveFinishedWorksWorkerToGenerator();
	CheckPlayerPosition();
	GenerateChunk();
	GenerateChunkMesh();
	UpdateTerrain();

	if (bDebug)
	{
		for (const TPair<FIntVector, UTerrainChunk*>& Chunk : Chunks)
		{
			if (Chunk.Value->GetDirty())
				DrawDebugBox(GetWorld(), ChunkToWorld(Chunk.Key), (FVector(ChunkScale) * FVector(ChunkSize) / 2.0f), FColor::Green, false, -1, 0, 50.0f);

			if(Chunk.Value->bUpdatingVoxel)
				DrawDebugBox(GetWorld(), ChunkToWorld(Chunk.Key), (FVector(ChunkScale) * FVector(ChunkSize) / 2.0f), FColor::Yellow, false, -1, 0, 50.0f);
		}
	}
}

void ATerrainGenerator::MoveFinishedWorksWorkerToGenerator()
{
	if (!TerrainWorker)
	{
		return;
	}

	FTerrainWorkerInformation FinishedWork;
	while (TerrainWorker->Dequeue(FinishedWork))
	{
		QueuedFinishedWorks.Enqueue(FinishedWork);
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

	FTerrainChunkWorkerInformation FinishedWork;
	if (ChunkWorker->Dequeue(FinishedWork))
	{
		if (UTerrainChunk* TerrainChunk = GetChunk(FinishedWork.ChunkLocation))
		{
			if (TerrainMaterial)
			{
				TerrainChunk->SetMaterial(0, TerrainMaterial);
			}
			else
			{
				TerrainChunk->SetMaterial(0, UMaterial::GetDefaultMaterial(MD_Surface));
			}

			for (int32 Index = 0; Index < FinishedWork.Voxels.Num(); Index++)
			{
				TerrainChunk->Voxels[Index] = (new FVoxel(FinishedWork.Voxels[Index]));
			}

			TerrainChunk->ChunkLocation = FinishedWork.ChunkLocation;
			TerrainChunk->SetDirty(true);
			TerrainChunk->bUpdatingVoxel = false;

			for (auto & ChunkLocation : FinishedWork.EdgeChunks)
			{
				if (UTerrainChunk* Chunk = GetChunk(ChunkLocation))
				{
					Chunk->SetDirty(true);
				}
			}
		}
	}
}

void ATerrainGenerator::GenerateChunkMesh()
{
	if (!TerrainWorker)
	{
		return;
	}

	FTerrainWorkerInformation FinishedWork;
	while(QueuedFinishedWorks.Dequeue(FinishedWork))
	{
		UTerrainChunk** FoundChunk = Chunks.Find(FinishedWork.ChunkLocation);
		if (!FoundChunk)
		{
			return;
		}
		UTerrainChunk *TerrainChunk = *FoundChunk;
		TerrainChunk->bUpdatingMesh = false;

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
		if (TerrainChunk->GetDirty())
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Regenerate Mesh, Outdated"), FinishedWork.Vertices.Num()));
			UpdateChunk(TerrainChunk->ChunkLocation);
			return;
		}

		TerrainChunk->SetArgent(false);
	}
}

void ATerrainGenerator::BeginDestroy()
{
	Super::BeginDestroy();
}

bool ATerrainGenerator::CreateChunk(FIntVector ChunkLocation)
{
	if (Chunks.Contains(ChunkLocation))
	{
		// 청크가 그 위치에 이미 있으면 만들지 않는다
		return false;
	}

	TArray<FVoxel*> Voxels;
	Voxels.Reserve((ChunkSize.X + 2) * (ChunkSize.Y + 2) * (ChunkSize.Z + 2));

	FTerrainChunkWorkerInformation Information;
	Information.Voxels = Voxels;
	Information.ChunkLocation = ChunkLocation;
	Information.ChunkSize = ChunkSize;
	Information.Generator = this;

	FString ComponentName;
	int32 ID = Chunks.Num();
	ComponentName.Append(TEXT("Chunk"));
	ComponentName.AppendInt(ID);
	FName ChunkComponentName;
	ChunkComponentName.AppendString(ComponentName);
	UTerrainChunk* NewChunk = NewObject<UTerrainChunk>(this, ChunkComponentName);
	NewChunk->RegisterComponent();
	NewChunk->SetGenerator(this);
	NewChunk->bUpdatingVoxel = true;
	Chunks.Add(ChunkLocation, NewChunk);

	ChunkWorker->Enqueue(Information);
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
	if (TerrainChunk->bUpdatingMesh)
	{
		// 청크가 이미 업데이트 중이면 하지 않고, 다음에 업데이트하도록 표시한다
		TerrainChunk->SetDirty(true);
		return false;
	}


	// 이웃 청크가 업데이트중이라면 하지 않는다.
	for (int32 X = -1; X <= 1; X++)
	{
		for (int32 Y = -1; Y <= 1; Y++)
		{
			for (int32 Z = -1; Z <= 1; Z++)
			{
				FIntVector NeighborChunkLocation = ChunkLocation + FIntVector(X, Y, Z);

				if (UTerrainChunk* NeighborChunk = GetChunk(NeighborChunkLocation))
				{
					if (NeighborChunk->bUpdatingVoxel)
					{
						return false;
					}
				}
			}
		}
	}
	TerrainChunk->bUpdatingMesh = true;
	TerrainChunk->SetDirty(false);
	

	TArray<FVoxel*> Voxels;
	Voxels.Reserve((ChunkSize.X + 2) * (ChunkSize.Y + 2) * (ChunkSize.Z + 2));
	{
		SCOPE_CYCLE_COUNTER(STAT_CopyVoxelDataToTerrainWorker);

		for (int32 Index = 0; Index < TerrainChunk->Voxels.Num(); Index++)
		{
			Voxels.Add(new FVoxel(TerrainChunk->Voxels[Index]));
		}
	}

	FTerrainWorkerInformation Information;
	Information.Chunk = TerrainChunk;
	Information.ChunkLocation = ChunkLocation;
	Information.ChunkScale = ChunkScale;
	Information.ChunkSize = ChunkSize;
	Information.IsoValue = IsoValue;
	Information.Generator = this;
	Information.Voxels = Voxels;

	if (TerrainChunk->GetArgent())
	{
		FTerrainWorker::GenerateSurfaceByMarchingCubes(Information.Voxels , Information.Chunk, Information.IsoValue, Information.ChunkLocation, Information.ChunkSize, Information.ChunkScale, Information.Vertices, Information.Indices, Information.Normals, Information.UVs, Information.VertexColors, Information.Tangents);
		Information.bValid = true;
		QueuedFinishedWorks.Enqueue(Information);
	}
	else
	{
		TerrainWorker->Enqueue(Information);
	}
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
	int32 ChunkX = FMath::FloorToInt((float)GridLocation.X / ChunkSize.X);
	int32 ChunkY = FMath::FloorToInt((float)GridLocation.Y / ChunkSize.Y);
	int32 ChunkZ = FMath::FloorToInt((float)GridLocation.Z / ChunkSize.Z);

	UTerrainChunk* TerrainChunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
	if (!TerrainChunk)
	{
		CreateChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
		return false;
	}

	TerrainChunk->SetVoxel(GridLocation, Value);

	if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f)
	{
		if (bLateUpdate)
		{
			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX - 1, ChunkY, ChunkZ)))
			{
				Chunk->SetVoxel(GridLocation, Value);
				Chunk->SetDirty(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ)))
			{
				Chunk->SetVoxel(GridLocation, Value);
				Chunk->SetDirty(true);
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
				Chunk->SetVoxel(GridLocation, Value);
				Chunk->SetDirty(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ)))
			{
				Chunk->SetVoxel(GridLocation, Value);
				Chunk->SetDirty(true);
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
				Chunk->SetVoxel(GridLocation, Value);
				Chunk->SetDirty(true);
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1)))
			{
				Chunk->SetVoxel(GridLocation, Value);
				Chunk->SetDirty(true);
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
		TerrainChunk->SetDirty(true);
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

bool ATerrainGenerator::AddVoxel(FIntVector GridLocation, float Value, bool bArgent)
{
	int32 ChunkX = FMath::FloorToInt((float)GridLocation.X / ChunkSize.X);
	int32 ChunkY = FMath::FloorToInt((float)GridLocation.Y / ChunkSize.Y);
	int32 ChunkZ = FMath::FloorToInt((float)GridLocation.Z / ChunkSize.Z);
	FIntVector ChunkLocation = FIntVector(ChunkX, ChunkY, ChunkZ);

	UTerrainChunk* TerrainChunk = GetChunk(ChunkLocation);
	if (!TerrainChunk)
	{
		CreateChunk(ChunkLocation);
		return false;
	}

	TerrainChunk->AddVoxel(GridLocation, Value);
	TerrainChunk->SetDirty(true);
	TerrainChunk->SetArgent(bArgent);

	if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f)
	{
		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX - 1, ChunkY, ChunkZ)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}

		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}
	}

	if (FMath::Fmod(GridLocation.Y, ChunkSize.Y) == 0.0f)
	{
		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY - 1, ChunkZ)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}

		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}
	}

	if (FMath::Fmod(GridLocation.Z, ChunkSize.Z) == 0.0f)
	{
		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ + 1)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}

		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}
	}

	if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f && FMath::Fmod(GridLocation.Y, ChunkSize.Y) == 0.0f)
	{
		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY + 1, ChunkZ)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}

		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX - 1, ChunkY - 1, ChunkZ)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}
	}

	if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f && FMath::Fmod(GridLocation.Z, ChunkSize.Z) == 0.0f)
	{
		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY , ChunkZ + 1)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}

		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX - 1, ChunkY, ChunkZ - 1)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}
	}

	if (FMath::Fmod(GridLocation.Y, ChunkSize.Y) == 0.0f && FMath::Fmod(GridLocation.Z, ChunkSize.Z) == 0.0f)
	{
		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ + 1)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}

		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY - 1, ChunkZ - 1)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}
	}

	if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f && FMath::Fmod(GridLocation.Y, ChunkSize.Y) == 0.0f && FMath::Fmod(GridLocation.Z, ChunkSize.Z) == 0.0f)
	{
		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY + 1, ChunkZ + 1)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}

		if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX - 1, ChunkY - 1, ChunkZ - 1)))
		{
			if (Chunk->AddVoxel(GridLocation, Value))
			{
				Chunk->SetDirty(true);
				Chunk->SetArgent(bArgent);
			}
		}
	}


	return true;
}

void ATerrainGenerator::AddVoxels(const TArray<FIntVector>& GridLocations, float Value)
{
	for (auto & GridLocation : GridLocations)
	{
		AddVoxel(GridLocation, Value, false);
	}
}

void ATerrainGenerator::AddVoxelArray(const TArray<FIntVector>& GridLocations, const TArray<float>& Values)
{
	if (GridLocations.Num() != Values.Num())
		return;

	for (int32 Index = 0; Index < GridLocations.Num(); Index++)
	{
		AddVoxel(GridLocations[Index], Values[Index], false);
	}
}

float ATerrainGenerator::GetVoxel(FIntVector GridLocation)
{
	int32 ChunkX = FMath::FloorToInt((float)GridLocation.X / ChunkSize.X);
	int32 ChunkY = FMath::FloorToInt((float)GridLocation.Y / ChunkSize.Y);
	int32 ChunkZ = FMath::FloorToInt((float)GridLocation.Z / ChunkSize.Z);

	if (UTerrainChunk* TerrainChunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ)))
	{
		return TerrainChunk->GetVoxelDensity(GridLocation);
	}

	return 0;
}

void ATerrainGenerator::UpdateTerrain()
{
	for (const TPair<FIntVector, UTerrainChunk*> Chunk : Chunks)
	{
		if (Chunk.Value->GetDirty())
		{
			UpdateChunk(Chunk.Key);
		}
	}
}

FIntVector ATerrainGenerator::WorldToGrid(FVector WorldLocation)
{
	int32 X = FMath::FloorToInt((WorldLocation.X + ChunkScale.X * 0.5f) / ChunkScale.X);
	int32 Y = FMath::FloorToInt((WorldLocation.Y + ChunkScale.Y * 0.5f) / ChunkScale.Y);
	int32 Z = FMath::FloorToInt((WorldLocation.Z + ChunkScale.Z * 0.5f) / ChunkScale.Z);

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

int32 ATerrainGenerator::To1DIndex(FIntVector Location, FIntVector ChunkSize)
{
	return Location.Z + Location.Y * ChunkSize.Z + Location.X * ChunkSize.Z * ChunkSize.Y;
}

FIntVector ATerrainGenerator::To3DIndex(int32 Index, FIntVector ChunkSize)
{
	int32 Z = Index % ChunkSize.Z;
	int32 Y = (Index / ChunkSize.Z) % ChunkSize.Y;
	int32 X = Index / (ChunkSize.Y * ChunkSize.Z);
	return FIntVector(X, Y, Z);
}
