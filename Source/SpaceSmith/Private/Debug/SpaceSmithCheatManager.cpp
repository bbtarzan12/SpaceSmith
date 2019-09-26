// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSmithCheatManager.h"
#include "SpaceSmithGameMode.h"
#include "ItemDataTable.h"
#include "BaseItem.h"
#include "SpaceSmithCharacterController.h"

void USpaceSmithCheatManager::SpawnItem(FName Name)
{
	SpawnItemWithNum(Name, 1);
}

void USpaceSmithCheatManager::SpawnItemWithNum(FName Name, int32 NumItem)
{
	if (!GetWorld())
		return;

	if (ASpaceSmithGameMode* GameMode = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (FItemRow* ItemRow = GameMode->ItemDataTable->FindRow<FItemRow>(Name, FString("Can not Found Item from FItemRow")))
		{
			FVector Location = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
			FRotator Rotation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorRotation();

			for (int32 Num = 0; Num < NumItem; Num++)
			{
				Location.Z += Num * 25.0f;
				if (ABaseItem* ItemActor = GetWorld()->SpawnActor<ABaseItem>(ItemRow->Class, Location, Rotation))
				{
					ItemActor->Initialize(*ItemRow);
				}
			}
		}
	}
}

void USpaceSmithCheatManager::AddItemToInventory(FName Name, int32 NumItem)
{
	if (!GetWorld())
		return;

	if (ASpaceSmithGameMode* GameMode = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (FItemRow* ItemRow = GameMode->ItemDataTable->FindRow<FItemRow>(Name, FString("Can not Found Item from FItemRow")))
		{
			if (ASpaceSmithCharacterController* Controller = Cast<ASpaceSmithCharacterController>(GetWorld()->GetFirstPlayerController()))
			{
				for (int32 Num = 0; Num < NumItem; Num++)
				{
					if (ABaseItem* ItemActor = GetWorld()->SpawnActor<ABaseItem>(ItemRow->Class, Controller->GetPawn()->GetActorLocation(), Controller->GetPawn()->GetActorRotation()))
					{
						ItemActor->Initialize(*ItemRow);
						Controller->AddItemToInventory(ItemActor);
					}
				}
			}
		}
	}
}
