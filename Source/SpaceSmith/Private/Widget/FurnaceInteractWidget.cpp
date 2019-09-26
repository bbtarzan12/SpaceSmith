// Fill out your copyright notice in the Description page of Project Settings.


#include "FurnaceInteractWidget.h"
#include "Furnace.h"


float UFurnaceInteractWidget::GetCurrentFuelPercent() const
{
	if (Furnace)
	{
		return Furnace->GetFuelPercent();
	}

	return 0;
}

float UFurnaceInteractWidget::GetEnergyPercent() const
{
	if (Furnace)
	{
		return Furnace->GetEnergyPercent();
	}

	return 0;
}
