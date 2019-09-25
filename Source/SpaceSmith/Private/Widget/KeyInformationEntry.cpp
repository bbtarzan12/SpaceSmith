// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyInformationEntry.h"
#include "Public/Widget/KeyInformationWidget.h"

void UKeyInformationEntry::NativeOnListItemObjectSet(UObject* InObject)
{
	if (UKeyInformation* NewInformation = Cast<UKeyInformation>(InObject))
	{
		Information = NewInformation;
	}
}

FText UKeyInformationEntry::GetKeyText() const
{
	if (Information)
	{
		return Information->Key;
	}
	else
	{
		return FText::GetEmpty();
	}
}

FText UKeyInformationEntry::GetKeyInformationText() const
{
	if (Information)
	{
		return Information->KeyInformation;
	}
	else
	{
		return FText::GetEmpty();
	}
}
