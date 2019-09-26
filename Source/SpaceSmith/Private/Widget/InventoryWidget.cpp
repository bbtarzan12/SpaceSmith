// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Public/SpaceSmithCharacterController.h"
#include <TileView.h>
#include "..\..\Public\Widget\InventoryWidget.h"
#include "Public/Component/InventoryComponent.h"

void UInventoryWidget::SetOwner(UInventoryComponent* NewOwner)
{
	OwnerComponent = NewOwner;
}

void UInventoryWidget::Add(UInventorySlot* InventorySlot)
{
	SlotTile->AddItem(InventorySlot);
}

void UInventoryWidget::Clear()
{
	SlotTile->ClearListItems();
}

FText UInventoryWidget::GetName() const
{
	if (OwnerComponent)
	{
		return OwnerComponent->GetInventoryName();
	}
	else
	{
		return FText::GetEmpty();
	}
}
