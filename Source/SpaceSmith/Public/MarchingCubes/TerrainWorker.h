// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Runnable.h>
#include <../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h>

DECLARE_STATS_GROUP(TEXT("Terrain Worker"), STATGROUP_TerrainWorker, STATCAT_Advanced);

class UTerrainData;
class UTerrainChunk;

struct FTerrainWorkerInformation
{
	UTerrainData* Grid;
	UTerrainChunk* Chunk;
	FIntVector ChunkLocation;
	FIntVector ChunkSize;
	FVector ChunkScale;
	float IsoValue;

	TArray<FVector> Vertices;
	TArray<int32> Indices;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;
	bool bValid;
};

/**
 * 
 */
class SPACESMITH_API FTerrainWorker : public FRunnable
{
public:
	FTerrainWorker();
	virtual ~FTerrainWorker();

	void EnsureCompletion();
	void Shutdown();

	bool Start();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	static int32 GenerateSurface
	(
		UTerrainData* Grid,
		UTerrainChunk* Chunk,
		float IsoValue,
		const FIntVector& ChunkLocation,
		const FIntVector& ChunkSize,
		const FVector& ChunkScale,

		TArray<FVector>& Vertices,
		TArray<int32>& Indices,
		TArray<FVector>& Normals,
		TArray<FVector2D>& UVs,
		TArray<FColor>& VertexColors,
		TArray<FProcMeshTangent>& Tangents
	);

private:
	static FVector VectorInterp(float IsoValue, FVector P1, FVector P2, float ValueP1, float ValueP2);

public:
	// 贸府且 没农狼 沥焊
	TQueue <FTerrainWorkerInformation> QueuedWorks;

	// 贸府啊 场抄 没农狼 沥焊
	TQueue <FTerrainWorkerInformation> FinishedWorks;

private:
	FThreadSafeCounter StopTaskCounter;
	FRunnableThread* Thread;

	bool bRunning;

	static const int32 EdgeTable[256];
	static const int32 TriTable[];
	
};
