// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachine.h"
#include "Furnace.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API AFurnace : public ABaseMachine
{
	GENERATED_BODY()
	
public:
	AFurnace();

	virtual bool Interact_Implementation(ASpaceSmithCharacterController* Controller) override;
	FORCEINLINE float GetFuelPercent() const { return CurrentFuel / MaxFuel; }
	FORCEINLINE float GetEnergyPercent() const { return Energy / MaxEnergy; }

protected:
	virtual void BeginPlay() override;
	virtual void RunningTick(float DeltaTime) override;

private:
	void Fuel();
	void Crafting();

private:
	UPROPERTY(VisibleAnywhere)
	class UFurnaceInteractWidget* InteractWidget;

	TSubclassOf<class UUserWidget> InteractWidgetClass;

	UPROPERTY(VisibleAnywhere)
	bool bInteractWidgetOpen;

	UPROPERTY(VisibleAnywhere)
	float Energy;

	UPROPERTY(VisibleAnywhere)
	float CurrentFuel;

	UPROPERTY(VisibleAnywhere)
	float MaxFuel = 1000;

	UPROPERTY(VisibleAnywhere)
	float MaxEnergy = 1000;

};
