// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "TerrainChunk.generated.h"

struct FVoxel;

/**
 * 
 */
UCLASS()
class SPACESMITH_API UTerrainChunk : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public:
	UTerrainChunk(const FObjectInitializer & ObjectInitializer);

	void SetGenerator(class ATerrainGenerator* NewGenerator);
	void SetDirty(bool bNewDirty);
	bool GetDirty();

	void SetArgent(bool bNewArgent);
	bool GetArgent();

	void SetVoxel(FIntVector WorldGridLocation, float Value);
	bool AddVoxel(FIntVector WorldGridLocation, float Value);
	float GetVoxelDensity(FIntVector WorldGridLocation);

public:
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	FIntVector ChunkLocation;

	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	bool bUpdatingMesh;

	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	bool bUpdatingVoxel;

	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	class ATerrainGenerator* Generator;

	TArray<FVoxel*> Voxels;

	FCriticalSection Mutex;

private:
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	bool bDirty;

	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	bool bArgent;
};
