// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerMasterWidget.generated.h"

class ASpaceSmithCharacterController;

/**
 * 
 */
UCLASS()
class SPACESMITH_API UPlayerMasterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetController(ASpaceSmithCharacterController* NewController);

	UPROPERTY(meta = (BindWidget))
	class UInventoryAddInformationWidget* InventoryAddInformation;

	UPROPERTY(meta = (BindWidget))
	class UPlayerInventoryWidget* Inventory;

	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* QuickBar;
	
	UPROPERTY(meta = (BindWidget))
	class UKeyInformationWidget* KeyInformation;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetKeyInformationVisibility() const;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetInventoryVisibility() const;

private:
	UPROPERTY(VisibleAnywhere)
	class ASpaceSmithCharacterController* Controller;	

};
