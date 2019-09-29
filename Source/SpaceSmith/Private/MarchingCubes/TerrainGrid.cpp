// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGrid.h"

UTerrainData::UTerrainData()
{

}

float UTerrainData::GetVoxel(FIntVector GridLocation)
{
	CriticalSection.Lock();
	if (GridData.Contains(GridLocation))
	{
		FVoxel* Voxel = GridData[GridLocation];
		CriticalSection.Unlock();
		return Voxel->Density;
	}
	CriticalSection.Unlock();
	return 0;
}

void UTerrainData::SetVoxel(FIntVector GridLocation, float Value)
{
	CriticalSection.Lock();
	FVoxel** Voxel = GridData.Find(GridLocation);
	if (Voxel)
	{
		delete (*Voxel);
	}

	GridData.Add(GridLocation, new FVoxel(Value));
	CriticalSection.Unlock();
}

UTerrainGrid::UTerrainGrid()
{
	Data = new UTerrainData();
}

void UTerrainGrid::SetVoxel(FIntVector GridLocation, float Value)
{
	if (Data)
		Data->SetVoxel(GridLocation, Value);
}

float UTerrainGrid::GetVoxel(FIntVector GridLocation)
{
	if (Data)
		return Data->GetVoxel(GridLocation);

	return 0;
}

void UTerrainGrid::BeginDestroy()
{
	if (Data)
	{
		delete Data;
	}

	Super::BeginDestroy();
}

UTerrainData* UTerrainGrid::GetDataProvider()
{
	return Data;
}
