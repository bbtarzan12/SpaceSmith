// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceSmithGameMode.generated.h"

UCLASS(minimalapi)
class ASpaceSmithGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpaceSmithGameMode();

public:
	class UDataTable* ItemDataTable;
};



