// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"

void UPlayerInventoryWidget::SetOwner(UInventoryComponent* NewOwner)
{
	Inventory->SetOwner(NewOwner);
}

void UPlayerInventoryWidget::Add(UInventorySlot* InventorySlot)
{
	Inventory->Add(InventorySlot);
}

void UPlayerInventoryWidget::Clear()
{
	Inventory->Clear();
}
