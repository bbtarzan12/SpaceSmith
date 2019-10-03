// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainGenerator.generated.h"

class UTerrainChunk;

UCLASS()
class SPACESMITH_API ATerrainGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrainGenerator();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	bool CreateChunk(FIntVector ChunkLocation);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	UTerrainChunk* GetChunk(FIntVector ChunkLocation);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	bool UpdateChunk(FIntVector ChunkLocation);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	bool DestroyChunk(FIntVector ChunkLocation);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	bool SetVoxel(FIntVector GridLocation, float Value, bool bLateUpdate = false);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	void SetVoxels(const TArray<FIntVector>& GridLocations, float Value);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	bool AddVoxel(FIntVector GridLocation, float Value, bool bLateUpdate = false);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	void AddVoxels(const TArray<FIntVector>& GridLocations, float Value);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	float GetVoxel(FIntVector GridLocation);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	void UpdateTerrain();

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	FIntVector WorldToGrid(FVector WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	FIntVector WorldToChunk(FVector WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	FVector ChunkToWorld(FIntVector ChunkLocation);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	FORCEINLINE FIntVector GetPlayerChunkPosition() const;

protected:
	virtual void BeginPlay() override;

private:
	void CheckPlayerPosition();
	void GenerateChunk();
	void GenerateChunkMesh();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	float IsoValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FVector ChunkScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FIntVector ChunkSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FIntVector NumSpawnChunk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	class UMaterialInterface* TerrainMaterial;

private:
	class FTerrainWorker* TerrainWorker;
	class FTerrainChunkWorker* ChunkWorker;
	class UTerrainData* Grid;

	UPROPERTY(VisibleAnywhere)
	class ASpaceSmithCharacter* Character;

	UPROPERTY(VisibleAnywhere)
	TMap<FIntVector, UTerrainChunk*> Chunks;

	UPROPERTY(VisibleAnywhere)
	FIntVector LastCharacterChunkLocation = FIntVector(-999999, -999999, -999999);

	UPROPERTY(VisibleAnywhere)
	TSet<FIntVector> CalculatingChunks; // 아직 만들어지지 않고, 계산중인 청크 위치

	UPROPERTY(EditAnywhere)
	bool bDebug;

};
