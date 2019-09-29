// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h>
#include "MarchingCubes.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UMarchingCubes : public UObject
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	static int32 GenerateSurface
	(
		float IsoValue,
		const FIntVector& ChunkStartSize,
		const FIntVector& ChunkEndSize,
		const FIntVector& ChunkLocation,
		const FVector& ChunkScale,
		const FTransform& TerrainTransform,

		TArray<FVector>& Vertices,
		TArray<int32>& Indices,
		TArray<FVector>& Normals,
		TArray<FVector2D>& UVs,
		TArray<FLinearColor>& VertexColors,
		TArray<FProcMeshTangent>& Tangents
	);

private:
	static FVector VectorInterp(float IsoValue, FVector P1, FVector P2, float ValueP1, float ValueP2);

private:
	static const int32 EdgeTable[256];
	static const int32 TriTable[];
};
