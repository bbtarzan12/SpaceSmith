// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <IUserObjectListEntry.h>
#include "ManufacturerInteractWidget.h"
#include "CraftingRuleDataTable.h"
#include <TileView.h>
#include "ManufacturerRuleEntry.generated.h"

UCLASS(BlueprintType)
class UManufacturerRuleEntryObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<UManufacturerRuleInformation*> Outs;

	UPROPERTY()
	FCraftingRuleRow Rule;

	UPROPERTY()
	AManufacturer* Manufacturer;
};

/**
 * 
 */
UCLASS()
class SPACESMITH_API UManufacturerRuleEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* InObject) override;

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(VisibleAnywhere)
	class AManufacturer* Manufacturer;

	UPROPERTY()
	FCraftingRuleRow Rule;

	UPROPERTY(meta = (BindWidget))
	class UTileView* RuleTileView;

	UPROPERTY(meta = (BindWidget))
	class UButton* SelectButton;

	UFUNCTION(BlueprintCallable)
	void Select();

	UFUNCTION(BlueprintCallable)
	bool GetButtonEnable() const;
};
