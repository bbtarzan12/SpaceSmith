// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainChunk.h"
#include "MarchingCubes/TerrainGenerator.h"

UTerrainChunk::UTerrainChunk()
{
	bUpdatingMesh = false;
	bHasChanges = false;
}

void UTerrainChunk::SetChanges(bool bChanges)
{
	Mutex.Lock();
	bHasChanges = bChanges;
	Mutex.Unlock();
}

bool UTerrainChunk::HasChanges()
{
	Mutex.Lock();
	bool bHasLocalChanges = bHasChanges;
	Mutex.Unlock();
	return bHasLocalChanges;
}