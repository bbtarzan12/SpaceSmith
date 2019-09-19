// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryWidget.h"
#include "Public/SpaceSmithCharacterController.h"
#include <TileView.h>
#include "..\..\Public\Widget\PlayerInventoryWidget.h"
#include "SpaceSmithGameMode.h"


FItemRow* UPlayerInventoryWidget::EmptyItemRow;

void UPlayerInventoryWidget::AddItem(UInventoryItem* Item)
{
	InventoryTile->AddItem(Item);
}

void UPlayerInventoryWidget::FillEmptyTiles(int32 NumEmptyTiles)
{
	if (EmptyItemRow == nullptr)
	{
		ASpaceSmithGameMode* GameMode = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode());
		EmptyItemRow = GameMode->ItemDataTable->FindRow<FItemRow>(TEXT("Empty"), FString("Can not Found Empty from FItemRow"));
		ensure(EmptyItemRow);
	}

	for (int32 Num = 0; Num < NumEmptyTiles; Num++)
	{
		UInventoryItem* NewItem = NewObject<UInventoryItem>();
		NewItem->Amount = 0;
		NewItem->Row = *EmptyItemRow;
		InventoryTile->AddItem(NewItem);
	}
}

void UPlayerInventoryWidget::ClearTiles()
{
	InventoryTile->ClearListItems();
}
