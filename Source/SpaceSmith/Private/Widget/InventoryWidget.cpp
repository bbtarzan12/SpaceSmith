// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Public/SpaceSmithCharacterController.h"
#include <TileView.h>
#include "..\..\Public\Widget\InventoryWidget.h"
#include "Public/Component/InventoryComponent.h"

void UInventoryWidget::Add(UInventorySlot* Slot)
{
	SlotTile->AddItem(Slot);
}

void UInventoryWidget::Clear()
{
	SlotTile->ClearListItems();
}
