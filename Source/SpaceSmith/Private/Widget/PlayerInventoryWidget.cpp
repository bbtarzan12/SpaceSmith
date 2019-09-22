// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryWidget.h"
#include "Public/SpaceSmithCharacterController.h"
#include <TileView.h>
#include "..\..\Public\Widget\PlayerInventoryWidget.h"
#include "Public/Component/InventoryComponent.h"

void UPlayerInventoryWidget::Add(UInventorySlot* Slot)
{
	SlotTile->AddItem(Slot);
}

void UPlayerInventoryWidget::Clear()
{
	SlotTile->ClearListItems();
}
