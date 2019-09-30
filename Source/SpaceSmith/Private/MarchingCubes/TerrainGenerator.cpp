// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGenerator.h"
#include "MarchingCubes/TerrainWorker.h"
#include <../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h>
#include "MarchingCubes/TerrainChunk.h"
#include "MarchingCubes/TerrainGrid.h"
#include "../Plugins/SimplexNoise/Source/SimplexNoise/Public/SimplexNoiseBPLibrary.h"

// Sets default values
ATerrainGenerator::ATerrainGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Worker = new FTerrainWorker();
	Worker->Start();

	Grid = new UTerrainData();
}

// Called when the game starts or when spawned
void ATerrainGenerator::BeginPlay()
{
	Super::BeginPlay();

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

							if(Density < SampleZ)
								SetVoxel(FIntVector(SampleX, SampleY, SampleZ), 1);
							else
								SetVoxel(FIntVector(SampleX, SampleY, SampleZ), -1);

						}
					}
				}
			}
		}
	}
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

		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Chunk(%s) Dequeue with %d Vertices"), *FinishedWork.ChunkLocation.ToString(), FinishedWork.Vertices.Num()));
		UTerrainChunk *TerrainChunk = *FoundChunk;
		TArray<FVector2D> UVs;
		TArray<FColor> VertexColors;
		TerrainChunk->ClearAllMeshSections();
		TerrainChunk->CreateMeshSection(0, FinishedWork.Vertices, FinishedWork.Indices, FinishedWork.Normals, FinishedWork.UVs, FinishedWork.VertexColors, FinishedWork.Tangents, true);
		TerrainChunk->bUpdating = false;

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
		// 청크가 그 위치에 이미 있으면 만들지 않는다
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
	Chunks.Add(ChunkLocation, NewChunk);
	UpdateChunk(ChunkLocation);
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
		// 청크가 이미 업데이트 중이면 하지 않는다
		TerrainChunk->bHasChanges = true;
		return false;
	}

	TerrainChunk->bUpdating = true;
	TerrainChunk->bHasChanges = false;

	FTerrainWorkerInformation Information;
	Information.Grid = Grid;
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

bool ATerrainGenerator::SetVoxel(FIntVector GridLocation, float Value, bool bCreateIfNotExists)
{
	Grid->SetVoxel(GridLocation, Value);

	int32 ChunkX = FMath::FloorToInt((float)GridLocation.X / ChunkSize.X);
	int32 ChunkY = FMath::FloorToInt((float)GridLocation.Y / ChunkSize.Y);
	int32 ChunkZ = FMath::FloorToInt((float)GridLocation.Z / ChunkSize.Z);

	if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f)
	{
		UpdateChunk(FIntVector(ChunkX - 1, ChunkY, ChunkZ));
		UpdateChunk(FIntVector(ChunkX + 1, ChunkY, ChunkZ));
	}

	if (FMath::Fmod(GridLocation.Y, ChunkSize.Y) == 0.0f)
	{
		UpdateChunk(FIntVector(ChunkX, ChunkY - 1, ChunkZ));
		UpdateChunk(FIntVector(ChunkX, ChunkY + 1, ChunkZ));
	}

	if (FMath::Fmod(GridLocation.Z, ChunkSize.Z) == 0.0f)
	{
		UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ + 1));
		UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ - 1));
	}

	UTerrainChunk* TerrainChunk = GetChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
	if (!TerrainChunk)
	{
		if (bCreateIfNotExists)
		{
			CreateChunk(FIntVector(ChunkX, ChunkY, ChunkZ));
		}
		return false;
	}

	UpdateChunk(FIntVector(ChunkX, ChunkY, ChunkZ));

	return true;
}

float ATerrainGenerator::GetVoxel(FIntVector GridLocation)
{
	if (Grid)
		return Grid->GetVoxel(GridLocation);

	return 0;
}