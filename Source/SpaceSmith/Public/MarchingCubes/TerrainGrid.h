// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TerrainGrid.generated.h"

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

public:
	TMap<FIntVector, FVoxel*> GridData;
	FCriticalSection CriticalSection;
	bool bChanges;

};

/**
 * 
 */
UCLASS()
class SPACESMITH_API UTerrainGrid : public UObject
{
	GENERATED_BODY()
	
public:
	UTerrainGrid();

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	void SetVoxel(FIntVector GridLocation, float Value);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	float GetVoxel(FIntVector GridLocation);

	virtual void BeginDestroy() override;

	UTerrainData* GetDataProvider();
	
private:
	UTerrainData* Data;


	
};
