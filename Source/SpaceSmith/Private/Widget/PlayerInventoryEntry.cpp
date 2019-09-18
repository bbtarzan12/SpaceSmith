// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryEntry.h"
#include "Public/SpaceSmithCharacterController.h"

void UPlayerInventoryEntry::SetListItemObjectInternal(UObject* InObject)
{
	Item = Cast<UInventoryItem>(InObject);
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
	return FText::AsNumber(Item->Amount);
}
