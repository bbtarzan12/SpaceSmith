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

protected:
	virtual void BeginPlay() override;
	virtual void RunningTick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	class UFurnaceInteractWidget* InteractWidget;

	TSubclassOf<class UUserWidget> InteractWidgetClass;

	UPROPERTY(VisibleAnywhere)
	bool bInteractWidgetOpen;

};
