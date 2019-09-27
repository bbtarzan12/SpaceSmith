// Fill out your copyright notice in the Description page of Project Settings.


#include "ManufacturerInformationEntry.h"
#include "ManufacturerInteractWidget.h"

void UManufacturerInformationEntry::NativeOnListItemObjectSet(UObject* InObject)
{
	if (UManufacturerRuleInformation* NewInformation = Cast<UManufacturerRuleInformation>(InObject))
	{
		Information = NewInformation;
	}
}

UTexture2D* UManufacturerInformationEntry::GetImage() const
{
	if (Information)
	{
		return Information->Row.Thumbnail;
	}

	return nullptr;
}

FText UManufacturerInformationEntry::GetName() const
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

FText UManufacturerInformationEntry::GetAmount() const
{
	if (Information)
	{
		return FText::Format(FTextFormat(FText::FromString(TEXT("{0} / {1}"))), FText::AsNumber(Information->RequireAmount), FText::AsNumber(Information->HasAmount));
	}
	return FText::GetEmpty();
}
