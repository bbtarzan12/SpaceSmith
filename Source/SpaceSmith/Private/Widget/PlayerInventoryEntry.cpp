// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryEntry.h"
#include "Public/SpaceSmithCharacterController.h"
#include "Public/Widget/DragAndDrop/InventorySlotDragOperation.h"
#include <WidgetBlueprintLibrary.h>

void UPlayerInventoryEntry::SetListItemObjectInternal(UObject* InObject)
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
		ASpaceSmithCharacterController* Controller = Cast<ASpaceSmithCharacterController>(GetWorld()->GetFirstPlayerController());
		Controller->SwapItem(ItemSlot, Cast<UInventorySlot>(DropOperation->Payload));
		return true;
	}
	return false;
}

FText UPlayerInventoryEntry::GetName() const
{
	return ItemSlot->Row.Name;
}

UTexture2D* UPlayerInventoryEntry::GetImage() const
{
	return ItemSlot->Row.Thumbnail;
}

FText UPlayerInventoryEntry::GetAmount() const
{
	if (ItemSlot->Amount <= 0)
		return FText::GetEmpty();
	return FText::AsNumber(ItemSlot->Amount);
}
