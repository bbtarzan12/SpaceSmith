// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainData.h"

UTerrainData::UTerrainData()
{

}

float UTerrainData::GetDensity(FIntVector GridLocation)
{
	CriticalSection.Lock();
	if (GridData.Contains(GridLocation))
	{
		float Density = GridData[GridLocation]->Density;
		CriticalSection.Unlock();
		return Density;
	}
	CriticalSection.Unlock();
	return 0;
}

void UTerrainData::SetDensity(FIntVector GridLocation, float Value)
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

float UTerrainData::AddDensity(FIntVector GridLocation, float Value)
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

float UTerrainData::GetHeight(FIntVector GridLocation)
{
	CriticalSection.Lock();
	if (GridData.Contains(GridLocation))
	{
		float TestValue = GridData[GridLocation]->Height;
		CriticalSection.Unlock();
		return TestValue;
	}
	CriticalSection.Unlock();
	return 0;
}

void UTerrainData::SetHeight(FIntVector GridLocation, float Value)
{
	CriticalSection.Lock();
	if (GridData.Contains(GridLocation))
	{
		GridData[GridLocation]->Height = Value;
	}
	else
	{
		GridData.Add(GridLocation, new FVoxel(0, Value));
	}
	CriticalSection.Unlock();
}
