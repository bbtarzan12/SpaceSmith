// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ResourceDataTable.h"
#include "CraftingRuleDataTable.h"
#include "SpaceSmithGameMode.generated.h"

UCLASS(minimalapi)
class ASpaceSmithGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpaceSmithGameMode();

public:
	UPROPERTY()
	class UDataTable* ItemDataTable;

	UPROPERTY()
	class UDataTable* ResourceDataTable;

	UPROPERTY()
	class UDataTable* FurnaceCraftingRuleDataTable;

	UPROPERTY()
	TMap<FString, FResourceRow> ResourceMap;

	UPROPERTY()
	TArray<FCraftingRuleRow> FurnaceCraftingRules;
};



