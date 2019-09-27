// Fill out your copyright notice in the Description page of Project Settings.


#include "ManufacturerInteractWidget.h"
#include "Manufacturer.h"
#include <Button.h>

bool UManufacturerInteractWidget::Initialize()
{
	bool success = Super::Initialize();
	if (!success)  return false;

	if (!ensure(CraftButton != nullptr)) return false;
	CraftButton->OnClicked.AddDynamic(this, &UManufacturerInteractWidget::Craft);
	UpButton->OnClicked.AddDynamic(this, &UManufacturerInteractWidget::UpAmount);
	DownButton->OnClicked.AddDynamic(this, &UManufacturerInteractWidget::DownAmount);
	return true;
}

float UManufacturerInteractWidget::GetEnergyPercent() const
{
	if (Manufacturer)
	{
		return Manufacturer->GetEnergyPercent();
	}

	return 0;
}

FText UManufacturerInteractWidget::GetCraftAmountText() const
{
	return FText::Format(FTextFormat(FText::FromString(TEXT("{0} / {1}"))), FText::AsNumber(NumCraft), FText::AsNumber(MaxCraft));
}

bool UManufacturerInteractWidget::GetCraftButtonEnable() const
{
	if (Manufacturer && NumCraft > 0)
	{
		return Manufacturer->CanCraftItems();
	}

	return false;
}

bool UManufacturerInteractWidget::GetUpButtonEnable() const
{
	return NumCraft < MaxCraft;
}

bool UManufacturerInteractWidget::GetDownButtonEnable() const
{
	return NumCraft > 0;
}

void UManufacturerInteractWidget::Craft()
{
	if (Manufacturer)
	{
		Manufacturer->StartCrafting();
	}
}

void UManufacturerInteractWidget::UpAmount()
{
	NumCraft = FMath::Clamp(NumCraft + 1, 0, MaxCraft);
}

void UManufacturerInteractWidget::DownAmount()
{
	NumCraft = FMath::Clamp(NumCraft - 1, 0, MaxCraft);
}
