// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryEntry.h"
#include "Public/SpaceSmithCharacterController.h"
#include "Public/Widget/DragAndDrop/InventorySlotDragOperation.h"
#include <WidgetBlueprintLibrary.h>

void UPlayerInventoryEntry::SetListItemObjectInternal(UObject* InObject)
{
	Item = Cast<UInventoryItem>(InObject);
}

void UPlayerInventoryEntry::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (UInventorySlotDragOperation* DragOperation = Cast<UInventorySlotDragOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventorySlotDragOperation::StaticClass())))
	{
		DragOperation->DefaultDragVisual = this;
		DragOperation->Payload = Item;
		OutOperation = DragOperation;
	}
}

bool UPlayerInventoryEntry::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UInventorySlotDragOperation* DropOperation = Cast<UInventorySlotDragOperation>(InOperation))
	{
		return true;
	}
	return false;
}

FText UPlayerInventoryEntry::GetName() const
{
	return Item->Row.Name;
}

UTexture2D* UPlayerInventoryEntry::GetImage() const
{
	return Item->Row.Thumbnail;
}

FText UPlayerInventoryEntry::GetAmount() const
{
	if (Item->Amount <= 0)
		return FText::GetEmpty();
	return FText::AsNumber(Item->Amount);
}
