// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerMasterWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UPlayerMasterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UPlayerInventoryWidget* Inventory;

	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* QuickBar;
	
	UPROPERTY(meta = (BindWidget))
	class UKeyInformationWidget* KeyInformation;

};
