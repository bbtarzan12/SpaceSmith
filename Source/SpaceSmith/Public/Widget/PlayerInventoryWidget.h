// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventoryWidget.generated.h"

class UInventorySlot;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class SPACESMITH_API UPlayerInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetOwner(UInventoryComponent* NewOwner);
	void Add(UInventorySlot* Slot);
	void Clear();

private:
	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* Inventory;

	
};
