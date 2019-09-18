// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SpaceSmithGameMode.h"
#include "SpaceSmithHUD.h"
#include "SpaceSmithCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASpaceSmithGameMode::ASpaceSmithGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASpaceSmithHUD::StaticClass();
}
