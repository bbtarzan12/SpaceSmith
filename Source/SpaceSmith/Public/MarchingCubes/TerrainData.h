// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

struct SPACESMITH_API FVoxel
{
	float Density;

	FVoxel(float Value)
	{
		Density = Value;
	}
};

class SPACESMITH_API UTerrainData
{

public:
	UTerrainData();

	float GetVoxel(FIntVector GridLocation);
	void SetVoxel(FIntVector GridLocation, float Value);
	float AddVoxel(FIntVector GridLocation, float Value);

public:
	TMap<FIntVector, FVoxel*> GridData;
	FCriticalSection CriticalSection;

};
