// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryEntry.h"
#include "Public/Widget/DragAndDrop/InventorySlotDragOperation.h"
#include <WidgetBlueprintLibrary.h>
#include "Public/Component/InventoryComponent.h"

void UPlayerInventoryEntry::NativeOnListItemObjectSet(UObject* InObject)
{
	ItemSlot = Cast<UInventorySlot>(InObject);
}

void UPlayerInventoryEntry::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (ItemSlot->Row.ItemID == 0)
		return;

	if (UInventorySlotDragOperation* DragOperation = Cast<UInventorySlotDragOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventorySlotDragOperation::StaticClass())))
	{
		DragOperation->DefaultDragVisual = this;
		DragOperation->Payload = ItemSlot;
		OutOperation = DragOperation;
	}
}

bool UPlayerInventoryEntry::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UInventorySlotDragOperation* DropOperation = Cast<UInventorySlotDragOperation>(InOperation))
	{
		UInventorySlot* ItemSlot2 = Cast<UInventorySlot>(DropOperation->Payload);
		UInventoryComponent::SwapItem(ItemSlot, ItemSlot2);
		return true;
	}
	return false;
}

FText UPlayerInventoryEntry::GetName() const
{
	if (ItemSlot == nullptr)
		return FText::GetEmpty();

	return ItemSlot->Row.Name;
}

UTexture2D* UPlayerInventoryEntry::GetImage() const
{
	if (ItemSlot == nullptr)
		return UTexture2D::CreateTransient(1, 1);
	return ItemSlot->Row.Thumbnail;
}

FText UPlayerInventoryEntry::GetAmount() const
{
	if (ItemSlot == nullptr || ItemSlot->Amount <= 0)
		return FText::GetEmpty();
	return FText::AsNumber(ItemSlot->Amount);
}

ESlateVisibility UPlayerInventoryEntry::IsSelected() const
{
	if (ItemSlot->bSelected)
	{
		return ESlateVisibility::Visible;
	}

	return ESlateVisibility::Hidden;
}
