// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DataTable/ItemDataTable.h"
#include "SpaceSmithCharacterController.generated.h"

UCLASS()
class UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FItemRow Row;

	UPROPERTY(EditAnywhere)
	int32 Amount;
};

/**
 * 
 */
UCLASS()
class SPACESMITH_API ASpaceSmithCharacterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASpaceSmithCharacterController();

	void ReloadInventory();
	bool AddItemToInventory(const FItemRow& ItemRow);
	void ToggleInventoryUMG();

protected:
	virtual void BeginPlay() override;


public:
	UPROPERTY(VisibleAnywhere)
	TArray<UInventoryItem*> Inventory;

private:
	UPROPERTY(VisibleAnywhere)
	class UPlayerInventoryWidget* InventoryWidget;

	TSubclassOf<class UUserWidget> InventoryWidgetClass;

	
};
