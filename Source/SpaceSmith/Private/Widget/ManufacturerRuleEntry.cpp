// Fill out your copyright notice in the Description page of Project Settings.


#include "ManufacturerRuleEntry.h"
#include "Manufacturer.h"
#include <TileView.h>
#include <Button.h>

void UManufacturerRuleEntry::NativeOnListItemObjectSet(UObject* InObject)
{
	if (UManufacturerRuleEntryObject* EntryObject = Cast<UManufacturerRuleEntryObject>(InObject))
	{
		Manufacturer = EntryObject->Manufacturer;
		Rule = EntryObject->Rule;
		RuleTileView->ClearListItems();
		
		for (auto& Out : EntryObject->Outs)
		{
			RuleTileView->AddItem(Out);
		}
	}
}

bool UManufacturerRuleEntry::Initialize()
{
	bool success = Super::Initialize();
	if (!success)  return false;

	if (!ensure(SelectButton != nullptr)) return false;
	SelectButton->OnClicked.AddDynamic(this, &UManufacturerRuleEntry::Select);
	return true;
}

void UManufacturerRuleEntry::Select()
{
	if (Manufacturer)
	{
		if (Manufacturer->IsMachineCrafting())
			return;

		Manufacturer->SelectRule(Rule);
	}
}

bool UManufacturerRuleEntry::GetButtonEnable() const
{
	if (Manufacturer)
	{
		return !Manufacturer->IsMachineCrafting();
	}

	return false;
}
