// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryWidget.h"
#include "Public/SpaceSmithCharacterController.h"
#include <TileView.h>

void UPlayerInventoryWidget::AddItem(UInventoryItem* Item)
{
	InventoryTile->AddItem(Item);
}

void UPlayerInventoryWidget::ClearTiles()
{
	InventoryTile->ClearListItems();
}
