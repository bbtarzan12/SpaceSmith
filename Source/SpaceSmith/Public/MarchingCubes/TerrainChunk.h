// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "TerrainChunk.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UTerrainChunk : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public:
	UTerrainChunk();

	void SetChanges(bool bChanges);
	bool HasChanges();

public:
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	FIntVector ChunkLocation;

	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	bool bUpdatingMesh;

	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	bool bUpdatingVoxel;

	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	class ATerrainGenerator* Generator;

	FCriticalSection Mutex;

private:
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	bool bHasChanges;
};
