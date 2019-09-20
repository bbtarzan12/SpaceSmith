// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineDataTable.h"

FMachineRow::FMachineRow()
{

}

bool FMachineRow::operator==(const FMachineRow& Other) const
{
	return ItemID == Other.ItemID;
}
