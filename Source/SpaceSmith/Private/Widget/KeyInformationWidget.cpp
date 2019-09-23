// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyInformationWidget.h"


void UKeyInformationWidget::Add(UKeyInformation* Information)
{
	InformationList->AddItem(Information);
}

void UKeyInformationWidget::Clear()
{
	InformationList->ClearListItems();
}
