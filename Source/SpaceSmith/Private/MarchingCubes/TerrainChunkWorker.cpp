// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainChunkWorker.h"
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
		Thread = FRunnableThread::Create(this, TEXT("TerrainChunkWorkerThread"), 0, TPri_AboveNormal);
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

				float Land = USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -1.0f, 1.0f, 0.005f);
				Land += USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -0.5f, 0.5f, 0.01f);
				Land += USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -0.25f, 0.25f, 0.02f);
				Land += USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -0.125f, 0.125f, 0.2f);
				
				float Mountain2D = USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, 0.0f, 30.0f, 0.01f);
				Mountain2D += USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -1.25f, 15.0f, 0.02f);
				Mountain2D += USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -0.625f, 7.0f, 0.04f);

				float MountainMask1 = USimplexNoiseBPLibrary::SimplexNoise012D(SampleX, SampleY, 0.001f);
				MountainMask1 *= USimplexNoiseBPLibrary::SimplexNoise012D(SampleX, SampleY, 0.002f);

				float MountainMask2 = USimplexNoiseBPLibrary::SimplexNoise012D(SampleX, SampleY, 0.003f);
				MountainMask2 *= USimplexNoiseBPLibrary::SimplexNoise012D(SampleX, SampleY, 0.006f);

				Density += Land;
				Density += Mountain2D * MountainMask1 * MountainMask2;
				Density = FMath::Clamp(Density, 0.0f, 1.0f);

				FIntVector GridLocation = FIntVector(SampleX, SampleY, SampleZ);
				float Height = FMath::Clamp((SampleZ + 5) / 30.0f, 0.0f, 1.0f);

				Grid->SetVoxelDensity(GridLocation, Density);
				Grid->SetVoxelHeight(GridLocation, Height);

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
