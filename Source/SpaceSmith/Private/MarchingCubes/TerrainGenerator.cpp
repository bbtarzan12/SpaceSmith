// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGenerator.h"
#include "MarchingCubes/TerrainWorker.h"
#include <../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h>
#include "MarchingCubes/TerrainChunk.h"
#include "MarchingCubes/TerrainData.h"
#include "../Plugins/SimplexNoise/Source/SimplexNoise/Public/SimplexNoiseBPLibrary.h"

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

	Worker = new FTerrainWorker();
	Worker->Start();

	Grid = new UTerrainData();

	for (int32 X = 0; X < 5; X++)
	{
		for (int32 Y = 0; Y < 5; Y++)
		{
			for (int32 Z = -1; Z <= 1; Z++)
			{
				for (int32 GridX = 0; GridX < ChunkSize.X; GridX++)
				{
					for (int32 GridY = 0; GridY < ChunkSize.Y; GridY++)
					{
						for (int32 GridZ = 0; GridZ < ChunkSize.Z; GridZ++)
						{
							int32 SampleX = GridX + ChunkSize.X * X;
							int32 SampleY = GridY + ChunkSize.Y * Y;
							int32 SampleZ = GridZ + ChunkSize.Z * Z;

							float Density = USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -5.0f, 5.0f, 0.003f);
							Density += USimplexNoiseBPLibrary::SimplexNoiseInRange3D(SampleX, SampleY, SampleZ, -3.0f, 3.0f, 0.07f);
							Density *= USimplexNoiseBPLibrary::SimplexNoiseInRange3D(SampleX, SampleY, SampleZ, -0.5f, 0.5f, 0.03f);

							if(Density > SampleZ)
								SetVoxel(FIntVector(SampleX, SampleY, SampleZ), 1, true);
							else
								SetVoxel(FIntVector(SampleX, SampleY, SampleZ), -1, true);
						}
					}
				}
			}
		}
	}

	UpdateTerrain();
}

// Called every frame
void ATerrainGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Worker)
	{
		return;
	}

	if (Worker->FinishedWorks.IsEmpty())
	{
		return;
	}

	FTerrainWorkerInformation FinishedWork;
	if (Worker->FinishedWorks.Dequeue(FinishedWork))
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
		TerrainChunk->CreateMeshSection(0, FinishedWork.Vertices, FinishedWork.Indices, FinishedWork.Normals, FinishedWork.UVs, FinishedWork.VertexColors, FinishedWork.Tangents, true);

		if (TerrainChunk->bHasChanges)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Regenerate Mesh, Outdated"), FinishedWork.Vertices.Num()));
			UpdateChunk(TerrainChunk->ChunkLocation);
		}
	}
}

void ATerrainGenerator::BeginDestroy()
{
	if (Worker)
	{
		Worker->EnsureCompletion();
		Worker->Shutdown();
		delete Worker;
		Worker = nullptr;
	}
	Super::BeginDestroy();
}

bool ATerrainGenerator::CreateChunk(FIntVector ChunkLocation)
{
	if (Chunks.Contains(ChunkLocation))
	{
		// ûũ�� �� ��ġ�� �̹� ������ ������ �ʴ´�
		return false;
	}

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
	NewChunk->ChunkLocation = ChunkLocation;
	NewChunk->bHasChanges = true;
	Chunks.Add(ChunkLocation, NewChunk);
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
		// ûũ�� �̹� ������Ʈ ���̸� ���� �ʰ�, ������ ������Ʈ �Ѵ�
		TerrainChunk->bHasChanges = true;
		return false;
	}

	TerrainChunk->bUpdating = true;
	TerrainChunk->bHasChanges = false;

	FTerrainWorkerInformation Information;
	Information.Grid = Grid;
	Information.Chunk = TerrainChunk;
	Information.ChunkLocation = ChunkLocation;
	Information.ChunkScale = ChunkScale;
	Information.ChunkSize = ChunkSize;
	Information.IsoValue = IsoValue;

	Worker->QueuedWorks.Enqueue(Information);
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
	Grid->SetVoxel(GridLocation, Value);

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
				Chunk->bHasChanges = true;
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ)))
			{
				Chunk->bHasChanges = true;
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
				Chunk->bHasChanges = true;
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ)))
			{
				Chunk->bHasChanges = true;
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
				Chunk->bHasChanges = true;
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1)))
			{
				Chunk->bHasChanges = true;
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
		TerrainChunk->bHasChanges = true;
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

	UpdateTerrain();
}

bool ATerrainGenerator::AddVoxel(FIntVector GridLocation, float Value, bool bLateUpdate /*= false*/)
{
	Grid->AddVoxel(GridLocation, Value);

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
				Chunk->bHasChanges = true;
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ)))
			{
				Chunk->bHasChanges = true;
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
				Chunk->bHasChanges = true;
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ)))
			{
				Chunk->bHasChanges = true;
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
				Chunk->bHasChanges = true;
			}

			if (UTerrainChunk* Chunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1)))
			{
				Chunk->bHasChanges = true;
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
		TerrainChunk->bHasChanges = true;
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

	UpdateTerrain();
}

float ATerrainGenerator::GetVoxel(FIntVector GridLocation)
{
	if (Grid)
		return Grid->GetVoxel(GridLocation);

	return 0;
}

void ATerrainGenerator::UpdateTerrain()
{
	for (const TPair<FIntVector, UTerrainChunk*> Chunk : Chunks)
	{
		if (Chunk.Value->bHasChanges)
		{
			UpdateChunk(Chunk.Key);
		}
	}
}

FIntVector ATerrainGenerator::WorldToGrid(FVector WorldLocation)
{
	int32 GridX = FMath::FloorToInt(WorldLocation.X / ChunkScale.X);
	int32 GridY = FMath::FloorToInt(WorldLocation.Y / ChunkScale.Y);
	int32 GridZ = FMath::FloorToInt(WorldLocation.Z / ChunkScale.Z);

	return FIntVector(GridX, GridY, GridZ);
}
