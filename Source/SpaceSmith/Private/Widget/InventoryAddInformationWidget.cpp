// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryAddInformationWidget.h"

void UInventoryAddInformationWidget::Add(UInventoryAddInformation* Information)
{
	InformationList->AddItem(Information);
}

void UInventoryAddInformationWidget::Clear()
{
	InformationList->ClearListItems();
}
