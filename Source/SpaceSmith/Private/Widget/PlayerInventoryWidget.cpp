// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryWidget.h"
#include "Public/SpaceSmithCharacterController.h"
#include <TileView.h>
#include "..\..\Public\Widget\PlayerInventoryWidget.h"
#include "Public/Component/InventoryComponent.h"

void UPlayerInventoryWidget::AddSlot(UInventorySlot* Slot)
{
	SlotTile->AddItem(Slot);
}

void UPlayerInventoryWidget::ClearSlots()
{
	SlotTile->ClearListItems();
}
