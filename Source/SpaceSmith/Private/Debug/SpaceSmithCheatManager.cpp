// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSmithCheatManager.h"
#include "SpaceSmithGameMode.h"
#include "ItemDataTable.h"
#include "BaseItem.h"

void USpaceSmithCheatManager::SpawnItem(FName Name)
{
	if (!GetWorld())
		return;

	if (ASpaceSmithGameMode* GameMode = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (FItemRow* ItemRow = GameMode->ItemDataTable->FindRow<FItemRow>(Name, FString("Can not Found Item from FItemRow")))
		{
			FVector Location = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
			FRotator Rotation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorRotation();
			if (ABaseItem* ItemActor = GetWorld()->SpawnActor<ABaseItem>(ItemRow->Class, Location, Rotation))
			{
				ItemActor->Initialize(*ItemRow);
			}
		}
	}
}
