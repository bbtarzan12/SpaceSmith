// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachine.h"
#include "ManufacturerInteractWidget.h"
#include "CraftingRuleDataTable.h"
#include "Manufacturer.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API AManufacturer : public ABaseMachine
{
	GENERATED_BODY()
	
public:
	AManufacturer();

	virtual bool Interact_Implementation(ASpaceSmithCharacterController* Controller) override;
	FORCEINLINE float GetEnergyPercent() const { return Energy / MaxEnergy; }
	FORCEINLINE bool IsMachineCrafting() const { return bCrafting; }
	FORCEINLINE bool CanCraftItems() const { return bCanCraft && !bCrafting; }

	void SelectRule(FCraftingRuleRow Rule);
	void StartCrafting();

	UFUNCTION()
	void ReloadInventory();

protected:
	virtual void BeginPlay() override;
	virtual void RunningTick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	class UManufacturerInteractWidget* InteractWidget;

	TSubclassOf<class UUserWidget> InteractWidgetClass;

	UPROPERTY(VisibleAnywhere)
	TArray<UManufacturerRuleInformation*> RuleInformations;

	UPROPERTY(VisibleAnywhere)
	ASpaceSmithCharacterController* CurrentController;

	UPROPERTY(VisibleAnywhere)
	bool bInteractWidgetOpen;

	UPROPERTY(VisibleAnywhere)
	float Energy;

	UPROPERTY(VisibleAnywhere)
	float MaxEnergy;

	UPROPERTY(VisibleAnywhere)
	bool bCrafting;

	UPROPERTY(VisibleAnywhere)
	bool bCanCraft;

	UPROPERTY(VisibleAnywhere)
	FCraftingRuleRow SelectedRule;
	
};
