// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FurnaceInteractWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UFurnaceInteractWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
	
public:
	UPROPERTY(VisibleAnywhere)
	class AFurnace* Furnace;

	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* FurnaceCraftingInventory;

	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* FurnaceFuelInventory;

	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* PlayerInventory;

	UFUNCTION(BlueprintCallable)
	float GetCurrentFuelPercent() const;
	
	UFUNCTION(BlueprintCallable)
	float GetEnergyPercent() const;
};
