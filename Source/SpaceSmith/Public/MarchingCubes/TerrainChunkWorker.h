// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Runnable.h>

class UTerrainData;
class ATerrainGenerator;
struct FVoxel;

struct FTerrainChunkWorkerInformation
{
	TArray<FVoxel*> Voxels;
	ATerrainGenerator* Generator;
	FIntVector ChunkLocation;
	FIntVector ChunkSize;
	TArray<FIntVector> EdgeChunks;
};

/**
 * 
 */
class SPACESMITH_API FTerrainChunkWorker : public FRunnable
{
public:
	FTerrainChunkWorker(ATerrainGenerator* Outer);
	virtual ~FTerrainChunkWorker() override;

	void EnsureCompletion();
	void Shutdown();

	bool Start();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	void GenerateChunk
	(
		TArray<FVoxel*>& Voxels,
		const FIntVector& ChunkLocation,
		const FIntVector& ChunkSize,
		TArray<FIntVector>& EdgeChunks
	);

	void Enqueue(FTerrainChunkWorkerInformation Work);
	bool Dequeue(FTerrainChunkWorkerInformation& Work);

	// 贸府且 没农狼 沥焊
	TArray<FTerrainChunkWorkerInformation> QueuedWorks;

	// 贸府啊 场抄 没农狼 沥焊
	TArray<FTerrainChunkWorkerInformation> FinishedWorks;

private:
	struct ChunkInformationPredicate
	{
		bool operator()(const FTerrainChunkWorkerInformation& A, const FTerrainChunkWorkerInformation& B) const;
	};

	FThreadSafeCounter StopTaskCounter;
	FRunnableThread* Thread;
	bool bRunning;
	FCriticalSection Mutex;

	UPROPERTY()
	class UUFNNoiseGenerator* LandNoise;

	UPROPERTY()
	class UUFNNoiseGenerator* MountainNoise;

	UPROPERTY()
	class UUFNNoiseGenerator* MountainMaskNoise;
};
