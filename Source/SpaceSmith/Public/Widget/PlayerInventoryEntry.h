// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <IUserObjectListEntry.h>
#include "PlayerInventoryEntry.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UPlayerInventoryEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void SetListItemObjectInternal(UObject* InObject) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	UPROPERTY()
	class UInventoryItem* Item;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Amount;

	UFUNCTION(BlueprintCallable)
	FText GetName() const;

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetImage() const;

	UFUNCTION(BlueprintCallable)
	FText GetAmount() const;


};
