// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachine.h"
#include "StorageChest.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API AStorageChest : public ABaseMachine
{
	GENERATED_BODY()
	
public:
	AStorageChest();

	virtual bool Interact_Implementation(ASpaceSmithCharacterController* Controller) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	class UStorageInteractWidget* InteractWidget;

	TSubclassOf<class UUserWidget> InteractWidgetClass;

	UPROPERTY(VisibleAnywhere)
	bool bInteractWidgetOpen;
	
};
