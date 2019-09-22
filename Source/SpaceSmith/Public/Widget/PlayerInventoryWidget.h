// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventoryWidget.generated.h"

class UInventorySlot;

/**
 * 
 */
UCLASS()
class SPACESMITH_API UPlayerInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Clear();
	void Add(UInventorySlot* Slot);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UTileView* SlotTile;

};
