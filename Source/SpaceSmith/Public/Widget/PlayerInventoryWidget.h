// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventoryWidget.generated.h"

class UInventoryItem;

/**
 * 
 */
UCLASS()
class SPACESMITH_API UPlayerInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ClearTiles();
	void AddItem(UInventoryItem* Item);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UTileView* InventoryTile;

};
