// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataTable.h"


bool FItemRow::operator==(const FItemRow& Other) const
{
	return ItemID == Other.ItemID;
}

FItemRow::FItemRow()
{
	this->Name = FText::FromString("No Name");
	this->Description = FText::FromString("No Description");
}
