// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainChunk.h"
#include "MarchingCubes/TerrainGenerator.h"
#include "TerrainData.h"

UTerrainChunk::UTerrainChunk()
{
	bUpdatingMesh = false;
	bDirty = false;
}

void UTerrainChunk::SetGenerator(ATerrainGenerator* NewGenerator)
{
	if (!NewGenerator)
		return;

	Generator = NewGenerator;
	Voxels.Empty();
	Voxels.SetNumUninitialized((NewGenerator->ChunkSize.X + 2) * (NewGenerator->ChunkSize.Y + 2) * (NewGenerator->ChunkSize.Z + 2));
}

void UTerrainChunk::SetDirty(bool bNewDirty)
{
	Mutex.Lock();
	bDirty = bNewDirty;
	Mutex.Unlock();
}

bool UTerrainChunk::GetDirty()
{
	Mutex.Lock();
	bool bTempDirty = bDirty;
	Mutex.Unlock();
	return bTempDirty;
}

void UTerrainChunk::SetArgent(bool bNewArgent)
{
	Mutex.Lock();
	bArgent = bNewArgent;
	Mutex.Unlock();
}

bool UTerrainChunk::GetArgent()
{
	Mutex.Lock();
	bool bTempArgent = bArgent;
	Mutex.Unlock();
	return bTempArgent;
}

void UTerrainChunk::SetVoxel(FIntVector WorldGridLocation, float Value)
{
	FIntVector ChunkSize = Generator->ChunkSize;
	FIntVector LocalGridLocation = FIntVector
	(
		(WorldGridLocation.X + 1) - ChunkLocation.X * ChunkSize.X,
		(WorldGridLocation.Y + 1) - ChunkLocation.Y * ChunkSize.Y,
		(WorldGridLocation.Z + 1) - ChunkLocation.Z * ChunkSize.Z
	);

	int32 Index = ATerrainGenerator::To1DIndex(LocalGridLocation, ChunkSize + FIntVector(2, 2, 2));
	Voxels[Index]->Density = Value;
}

bool UTerrainChunk::AddVoxel(FIntVector WorldGridLocation, float Value)
{
	FIntVector ChunkSize = Generator->ChunkSize;
	FIntVector LocalGridLocation = FIntVector
	(
		(WorldGridLocation.X + 1) - ChunkLocation.X * ChunkSize.X,
		(WorldGridLocation.Y + 1) - ChunkLocation.Y * ChunkSize.Y,
		(WorldGridLocation.Z + 1) - ChunkLocation.Z * ChunkSize.Z
	);

	if (LocalGridLocation.X < 0 || LocalGridLocation.Y < 0 || LocalGridLocation.Z < 0)
		return false;

	int32 Index = ATerrainGenerator::To1DIndex(LocalGridLocation, ChunkSize + FIntVector(2, 2, 2));
	Voxels[Index]->Density += Value;
	return true;
}

float UTerrainChunk::GetVoxelDensity(FIntVector WorldGridLocation)
{
	FIntVector ChunkSize = Generator->ChunkSize;
	FIntVector LocalGridLocation = FIntVector
	(
		(WorldGridLocation.X + 1) - ChunkLocation.X * ChunkSize.X,
		(WorldGridLocation.Y + 1) - ChunkLocation.Y * ChunkSize.Y,
		(WorldGridLocation.Z + 1) - ChunkLocation.Z * ChunkSize.Z
	);

	if (FMath::Fmod(LocalGridLocation.X, ChunkSize.X) == 0.0f)
	{
		LocalGridLocation.X = ChunkSize.X - LocalGridLocation.X;
	}

	if (FMath::Fmod(LocalGridLocation.Y, ChunkSize.Y) == 0.0f)
	{
		LocalGridLocation.Y = ChunkSize.Y - LocalGridLocation.Y;
	}
	if (FMath::Fmod(LocalGridLocation.Z, ChunkSize.Z) == 0.0f)
	{
		LocalGridLocation.Z = ChunkSize.Z - LocalGridLocation.Z;
	}

	int32 Index = ATerrainGenerator::To1DIndex(LocalGridLocation, ChunkSize + FIntVector(2, 2, 2));
	return Voxels[Index]->Density;
}
