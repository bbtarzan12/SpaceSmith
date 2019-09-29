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
	bool SetVoxel(FIntVector GridLocation, float Value, bool CreateIfNotExists);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	float GetVoxel(FIntVector GridLocation);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	float IsoValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FVector ChunkScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	FIntVector ChunkSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	class UMaterialInterface* TerrainMaterial;

private:
	class FTerrainWorker* Worker;
	class UTerrainData* Grid;

	UPROPERTY(VisibleAnywhere)
	TMap<FIntVector, UTerrainChunk*> Chunks;

};
