// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceOnDetectPopUpWidget.h"
#include "ItemDataTable.h"
#include "BaseResource.h"

UTexture2D* UResourceOnDetectPopUpWidget::GetTexture() const
{
	if (Resource)
	{
		if (FItemRow* ItemRow = Resource->Data.AbsorbInformation.Row.GetRow<FItemRow>(TEXT("Can not find ItemRow")))
		{
			return ItemRow->Thumbnail;
		}
	}

	return nullptr;
}
