// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MachineInformationWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UMachineInformationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	class ABaseMachine* Machine;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;

	UFUNCTION(BlueprintCallable)
	FText GetName() const;
	
};
