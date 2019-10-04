// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainData.h"

UTerrainData::UTerrainData()
{

}

float UTerrainData::GetDensity(FIntVector GridLocation)
{
	if (GridData.Contains(GridLocation))
	{
		return GridData[GridLocation]->Density;
	}
	return 0;
}

void UTerrainData::SetDensity(FIntVector GridLocation, float Value)
{
	if (GridData.Contains(GridLocation))
	{
		GridData[GridLocation]->Density = Value;
	}
	else
	{
		GridData.Add(GridLocation, new FVoxel(Value));
	}
}

float UTerrainData::AddDensity(FIntVector GridLocation, float Value)
{
	if (GridData.Contains(GridLocation))
	{
		GridData[GridLocation]->Density += Value;
		return GridData[GridLocation]->Density;
	}
	else
	{
		GridData.Add(GridLocation, new FVoxel(Value));
		return Value;
	}
}

float UTerrainData::GetHeight(FIntVector GridLocation)
{
	if (GridData.Contains(GridLocation))
	{
		return GridData[GridLocation]->Height;
	}
	return 0;
}

void UTerrainData::SetHeight(FIntVector GridLocation, float Value)
{
	if (GridData.Contains(GridLocation))
	{
		GridData[GridLocation]->Height = Value;
	}
	else
	{
		GridData.Add(GridLocation, new FVoxel(0, Value));
	}
}
