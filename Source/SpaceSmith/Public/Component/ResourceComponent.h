// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "ResourceComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UResourceComponent : public UFoliageInstancedStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	int32 TestValue;
	
};
