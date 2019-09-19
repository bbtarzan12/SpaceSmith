// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DataTable/ItemDataTable.h"
#include "SpaceSmithCharacterController.generated.h"

class ABaseItem;

UCLASS(BlueprintType)
class UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemRow Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
	bool AddItemToInventory(ABaseItem* AddingItem, bool Destroy = true);
	void ToggleInventoryUMG();

	UFUNCTION(BlueprintCallable)
	bool DropItemToWorld(const FItemRow& ItemRow, int32 Amount);

	FORCEINLINE bool GetInventoryVisible() const { return bInventoryVisible; }

protected:
	virtual void BeginPlay() override;


public:
	UPROPERTY(VisibleAnywhere)
	TArray<UInventoryItem*> Inventory;

private:
	UPROPERTY(VisibleAnywhere)
	class UPlayerMasterWidget* Widget;

	TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY(VisibleAnywhere)
	bool bInventoryVisible;

	UPROPERTY(VisibleAnywhere)
	int32 InventoryLimit = 30;

	
};
