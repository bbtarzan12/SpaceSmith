// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventorySlot;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class SPACESMITH_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetOwner(UInventoryComponent* NewOwner);
	void Add(UInventorySlot* Slot);
	void Clear();
	
private:
	UPROPERTY(VisibleAnywhere)
	UInventoryComponent* OwnerComponent;

	UPROPERTY(meta = (BindWidget))
	class UTileView* SlotTile;

	UFUNCTION(BlueprintCallable)
	FText GetName() const;

};
