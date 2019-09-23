// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMasterWidget.h"
#include "SpaceSmithCharacterController.h"
#include "KeyInformationWidget.h"

void UPlayerMasterWidget::SetController(ASpaceSmithCharacterController* NewController)
{
	Controller = NewController;
}

ESlateVisibility UPlayerMasterWidget::GetKeyInformationVisibility() const
{
	if (Controller)
	{
		if (Controller->GetViewportWidgetVisible() || KeyInformation->InformationList->GetNumItems() <= 0)
			return ESlateVisibility::Hidden;
		else
			return ESlateVisibility::Visible;
	}
	return ESlateVisibility::Hidden;
}

ESlateVisibility UPlayerMasterWidget::GetInventoryVisibility() const
{
	if (Controller)
	{
		if (Controller->GetInventoryVisible())
			return ESlateVisibility::Visible;
		else
			return ESlateVisibility::Hidden;
	}
	return ESlateVisibility::Hidden;
}
