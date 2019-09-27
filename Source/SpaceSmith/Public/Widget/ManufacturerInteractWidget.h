// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDataTable.h"
#include "ManufacturerInteractWidget.generated.h"

UCLASS(BlueprintType)
class UManufacturerRuleInformation : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemRow Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequireAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HasAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanCraft;
};

/**
 * 
 */
UCLASS()
class SPACESMITH_API UManufacturerInteractWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;
	
public:
	UPROPERTY(VisibleAnywhere)
	int32 NumCraft;

	UPROPERTY(VisibleAnywhere)
	int32 MaxCraft;

	UPROPERTY(VisibleAnywhere)
	class AManufacturer* Manufacturer;

	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* ManufacturerInventory;

	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* PlayerInventory;

	UPROPERTY(meta = (BindWidget))
	class UListView* CraftingRuleListView;
	
	UPROPERTY(meta = (BindWidget))
	class UTileView* InTileView;
	
	UPROPERTY(meta = (BindWidget))
	class UTileView* OutTileView;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CraftAmount;

	UPROPERTY(meta = (BindWidget))
	class UButton* CraftButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* UpButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* DownButton;

	UFUNCTION(BlueprintCallable)
	float GetEnergyPercent() const;

	UFUNCTION(BlueprintCallable)
	FText GetCraftAmountText() const;

	UFUNCTION(BlueprintCallable)
	bool GetCraftButtonEnable() const;

	UFUNCTION(BlueprintCallable)
	bool GetUpButtonEnable() const;

	UFUNCTION(BlueprintCallable)
	bool GetDownButtonEnable() const;

	UFUNCTION(BlueprintCallable)
	void Craft();

	UFUNCTION(BlueprintCallable)
	void UpAmount();

	UFUNCTION(BlueprintCallable)
	void DownAmount();
};
