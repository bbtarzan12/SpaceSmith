// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainWorker.h"
#include "TerrainGenerator.generated.h"

DECLARE_STATS_GROUP(TEXT("Terrain Generator"), STATGROUP_TerrainGenerator, STATCAT_Advanced);

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
	bool AddVoxel(FIntVector GridLocation, float Value, bool bArgent = false);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	void AddVoxels(const TArray<FIntVector>& GridLocations, float Value);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	void AddVoxelArray(const TArray<FIntVector>& GridLocations, const TArray<float>& Values);

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

	static int32 To1DIndex(FIntVector Location, FIntVector ChunkSize);
	static FIntVector To3DIndex(int32 Index, FIntVector ChunkSize);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void MoveFinishedWorksWorkerToGenerator();
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

	UPROPERTY(VisibleAnywhere)
	class ASpaceSmithCharacter* Character;

	UPROPERTY(VisibleAnywhere)
	TMap<FIntVector, UTerrainChunk*> Chunks;

	UPROPERTY(VisibleAnywhere)
	FIntVector LastCharacterChunkLocation = FIntVector(-999999, -999999, -999999);

	TQueue<FTerrainWorkerInformation> QueuedFinishedWorks;

	UPROPERTY(EditAnywhere)
	bool bDebug;

};
