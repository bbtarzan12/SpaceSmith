// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Select.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelect : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPACESMITH_API ISelect
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Select")
	bool Select(FHitResult HitResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Select")
	bool SelectTick(FHitResult HitResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Select")
	bool Deselect();
};
