// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <IUserObjectListEntry.h>
#include "InventoryAddInformationEntry.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UInventoryAddInformationEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void SetListItemObjectInternal(UObject* InObject) override;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* Image;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Amount;

	UPROPERTY(VisibleAnywhere)
	class UInventoryAddInformation* Information;

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetImage() const;

	UFUNCTION(BlueprintCallable)
	FText GetName() const;

	UFUNCTION(BlueprintCallable)
	FText GetAmount() const;
	
};