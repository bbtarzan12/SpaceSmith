// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "SpaceSmithCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API USpaceSmithCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
	UFUNCTION(exec)
	void SpawnItem(FName Name);

	UFUNCTION(exec)
	void SpawnItemWithNum(FName Name, int32 Num);
	
};
