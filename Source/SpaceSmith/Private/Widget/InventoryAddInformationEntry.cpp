// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryAddInformationEntry.h"
#include "InventoryAddInformationWidget.h"

void UInventoryAddInformationEntry::NativeOnListItemObjectSet(UObject* InObject)
{
	if (UInventoryAddInformation* NewInformation = Cast<UInventoryAddInformation>(InObject))
	{
		Information = NewInformation;
	}
}

UTexture2D* UInventoryAddInformationEntry::GetImage() const
{
	if (Information)
	{
		return Information->Row.Thumbnail;
	}
	else
	{
		return UTexture2D::CreateTransient(64, 64);
	}
}

FText UInventoryAddInformationEntry::GetName() const
{
	if (Information)
	{
		return Information->Row.Name;
	}
	else
	{
		return FText::GetEmpty();
	}
}

FText UInventoryAddInformationEntry::GetAmount() const
{
	if (Information)
	{
		return FText::Format(FTextFormat::FromString(TEXT("X {0}")), FText::AsNumber(Information->Amount));
	}
	else
	{
		return FText::GetEmpty();
	}
}
