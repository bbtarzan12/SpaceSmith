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

	static ConstructorHelpers::FObjectFinder<UDataTable> ItemDataTableFinder(TEXT("DataTable'/Game/SpaceSmith/Data/DataTable/DT_Item'"));
	if (ItemDataTableFinder.Succeeded())
	{
		ItemDataTable = ItemDataTableFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> ResourceDataTableFinder(TEXT("DataTable'/Game/SpaceSmith/Data/DataTable/DT_Resource'"));
	if (ResourceDataTableFinder.Succeeded())
	{
		ResourceDataTable = ResourceDataTableFinder.Object;

		TArray<FResourceRow*> Rows;
		ResourceDataTable->GetAllRows<FResourceRow>(TEXT("Can not find ResourceRows"), Rows);

		for (auto & Row : Rows)
		{
			if (Row)
			{
				ResourceMap.Emplace(Row->Mesh->GetName(), *Row);
			}
		}
	}
}