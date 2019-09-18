// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SpaceSmithGameMode.h"
#include "SpaceSmithHUD.h"
#include "SpaceSmithCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Public/SpaceSmithCharacterController.h"

ASpaceSmithGameMode::ASpaceSmithGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Blueprint'/Game/SpaceSmith/Characters/SpaceSmithCharacter'"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ASpaceSmithCharacterController::StaticClass();

	// use our custom HUD class
	HUDClass = ASpaceSmithHUD::StaticClass();
}
