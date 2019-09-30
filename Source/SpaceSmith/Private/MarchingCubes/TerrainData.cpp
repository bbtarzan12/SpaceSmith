// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainData.h"

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
	if (GridData.Contains(GridLocation))
	{
		GridData[GridLocation]->Density = Value;
	}
	else
	{
		GridData.Add(GridLocation, new FVoxel(Value));
	}
	CriticalSection.Unlock();
}

float UTerrainData::AddVoxel(FIntVector GridLocation, float Value)
{
	CriticalSection.Lock();
	if (GridData.Contains(GridLocation))
	{
		float Density = GridData[GridLocation]->Density;
		GridData[GridLocation]->Density += Value;
		CriticalSection.Unlock();
		return Density + Value;
	}
	else
	{
		GridData.Add(GridLocation, new FVoxel(Value));
		CriticalSection.Unlock();
		return Value;
	}
}
