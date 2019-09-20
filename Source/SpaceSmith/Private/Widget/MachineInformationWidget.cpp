// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineInformationWidget.h"
#include "Public/BaseMachine.h"

FText UMachineInformationWidget::GetName() const
{
	if(Machine)
		return Machine->GetName();

	return FText::GetEmpty();
}
