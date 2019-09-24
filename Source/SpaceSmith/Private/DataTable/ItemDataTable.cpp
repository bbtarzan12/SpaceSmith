// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataTable.h"
#include "BaseItem.h"


bool FItemRow::operator==(const FItemRow& Other) const
{
	return ItemID == Other.ItemID;
}

FItemRow::FItemRow()
{
	this->Name = FText::FromString("No Name");
	this->Description = FText::FromString("No Description");
	this->bStack = false;
	this->Class = ABaseItem::StaticClass();
}
