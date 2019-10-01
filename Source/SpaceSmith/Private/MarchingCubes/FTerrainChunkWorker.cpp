// Fill out your copyright notice in the Description page of Project Settings.


#include "FTerrainChunkWorker.h"
#include "MarchingCubes/TerrainData.h"
#include "../../../../../../Source/SimplexNoise/Public/SimplexNoiseBPLibrary.h"

FTerrainChunkWorker::FTerrainChunkWorker() : StopTaskCounter(0), Thread(nullptr), bRunning(false)
{
}

FTerrainChunkWorker::~FTerrainChunkWorker()
{
	delete Thread;
	Thread = nullptr;
}

void FTerrainChunkWorker::EnsureCompletion()
{
	if (!Thread)
		return;

	Stop();
	Thread->WaitForCompletion();
	while (bRunning)
	{
		FPlatformProcess::Sleep(1.00);
	}
}

void FTerrainChunkWorker::Shutdown()
{
	if (!Thread)
		return;

	Thread->Kill(true);
}

bool FTerrainChunkWorker::Start()
{
	if (FPlatformProcess::SupportsMultithreading())
	{
		Thread = FRunnableThread::Create(this, TEXT("TerrainChunkWorkerThread"), 0, TPri_BelowNormal);
		return true;
	}
	else
	{
		return false;
	}
}

bool FTerrainChunkWorker::Init()
{
	return true;
}

uint32 FTerrainChunkWorker::Run()
{
	bRunning = true;
	while (StopTaskCounter.GetValue() == 0)
	{
		if (QueuedWorks.Num() == 0)
		{
			continue;
		}

		FTerrainChunkWorkerInformation WorkerInformation;
		Mutex.Lock();
		QueuedWorks.HeapPop(WorkerInformation, ChunkInformationPredicate());
		Mutex.Unlock();
		GenerateChunk(WorkerInformation.Grid, WorkerInformation.ChunkLocation, WorkerInformation.ChunkSize, WorkerInformation.EdgeChunks);
		Mutex.Lock();
		FinishedWorks.HeapPush(WorkerInformation, ChunkInformationPredicate());
		Mutex.Unlock();
	}
	bRunning = false;
	return 0;
}

void FTerrainChunkWorker::Stop()
{
	if (!Thread)
		return;

	StopTaskCounter.Increment();
}

void FTerrainChunkWorker::GenerateChunk(UTerrainData* Grid, const FIntVector& ChunkLocation, const FIntVector& ChunkSize, TArray<FIntVector>& EdgeChunks)
{
	for (int32 X = 0; X < ChunkSize.X; X++)
	{
		for (int32 Y = 0; Y < ChunkSize.Y; Y++)
		{
			for (int32 Z = 0; Z < ChunkSize.Z; Z++)
			{
				int32 SampleX = X + ChunkSize.X * ChunkLocation.X;
				int32 SampleY = Y + ChunkSize.Y * ChunkLocation.Y;
				int32 SampleZ = Z + ChunkSize.Z * ChunkLocation.Z;

				float Density = -SampleZ;
				Density += USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -5.0f, 5.0f, 0.005f);
				Density += USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -3.0f, 3.0f, 0.01f);
				Density += USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -0.5f, 0.5f, 0.1f);
				Density = FMath::Clamp(Density, -1.0f, 1.0f);

				FIntVector GridLocation = FIntVector(SampleX, SampleY, SampleZ);

				Grid->SetVoxel(FIntVector(SampleX, SampleY, SampleZ), Density);

				if (FMath::Fmod(GridLocation.X, ChunkSize.X) == 0.0f)
				{
					EdgeChunks.Add(FIntVector(ChunkLocation.X - 1, ChunkLocation.Y, ChunkLocation.Z));
					EdgeChunks.Add(FIntVector(ChunkLocation.X + 1, ChunkLocation.Y, ChunkLocation.Z));
				}

				if (FMath::Fmod(GridLocation.Y, ChunkSize.Y) == 0.0f)
				{
					EdgeChunks.Add(FIntVector(ChunkLocation.X, ChunkLocation.Y - 1, ChunkLocation.Z));
					EdgeChunks.Add(FIntVector(ChunkLocation.X, ChunkLocation.Y + 1, ChunkLocation.Z));
				}

				if (FMath::Fmod(GridLocation.Z, ChunkSize.Z) == 0.0f)
				{
					EdgeChunks.Add(FIntVector(ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z + 1));
					EdgeChunks.Add(FIntVector(ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z - 1));
				}
			}
		}
	}
}

void FTerrainChunkWorker::Enqueue(FTerrainChunkWorkerInformation Work)
{
	Mutex.Lock();
	QueuedWorks.HeapPush(Work, ChunkInformationPredicate());
	Mutex.Unlock();
}

bool FTerrainChunkWorker::Dequeue(FTerrainChunkWorkerInformation& Work)
{
	if (FinishedWorks.Num() == 0)
		return false;
	Mutex.Lock();
	FinishedWorks.HeapPop(Work, ChunkInformationPredicate());
	Mutex.Unlock();
	return true;
}
