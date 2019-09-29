// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainChunk.h"
#include "MarchingCubes/TerrainGenerator.h"

UTerrainChunk::UTerrainChunk()
{
	bUpdating = false;
	bHasChanges = false;
}

void UTerrainChunk::RegenerateChunkMesh()
{
	if (Generator)
		Generator->UpdateChunk(ChunkLocation);
}
