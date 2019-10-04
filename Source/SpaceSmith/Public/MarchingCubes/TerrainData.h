// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

struct SPACESMITH_API FVoxel
{
	float Density;
	float Height;

	FVoxel()
	{
		Density = 0.0f;
		Height = 0.0f;
	}

	FVoxel(FVoxel* Voxel)
	{
		if (Voxel)
		{
			Density = Voxel->Density;
			Height = Voxel->Height;
		}
	}

	FVoxel(float NewDensity)
	{
		Density = NewDensity;
	}

	FVoxel(float NewDensity, float NewHeight)
	{
		Density = NewDensity;
		Height = NewHeight;
	}
};

class SPACESMITH_API UTerrainData
{

public:
	UTerrainData();

	float GetDensity(FIntVector GridLocation);
	void SetDensity(FIntVector GridLocation, float Value);
	float AddDensity(FIntVector GridLocation, float Value);

	float GetHeight(FIntVector GridLocation);
	void SetHeight(FIntVector GridLocation, float Value);

public:
	TMap<FIntVector, FVoxel*> GridData;

};
