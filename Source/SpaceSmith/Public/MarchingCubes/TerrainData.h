// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

struct SPACESMITH_API FVoxel
{
	float Density;
	float Height;

	FVoxel(float NewDensity)
	{
		Density = NewDensity;
	}

	FVoxel(float NewDensity, float NewTestValue)
	{
		Density = NewDensity;
		NewTestValue = NewTestValue;
	}
};

class SPACESMITH_API UTerrainData
{

public:
	UTerrainData();

	float GetVoxelDensity(FIntVector GridLocation);
	void SetVoxelDensity(FIntVector GridLocation, float Value);
	float AddVoxelDensity(FIntVector GridLocation, float Value);

	float GetVoxelHeight(FIntVector GridLocation);
	void SetVoxelHeight(FIntVector GridLocation, float Value);

public:
	TMap<FIntVector, FVoxel*> GridData;
	FCriticalSection CriticalSection;

};
