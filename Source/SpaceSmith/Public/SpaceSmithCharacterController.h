// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DataTable/ItemDataTable.h"
#include "SpaceSmithCharacter.h"
#include "InventoryAddInformationWidget.h"
#include "SpaceSmithCharacterController.generated.h"

class ABaseItem;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class SPACESMITH_API ASpaceSmithCharacterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASpaceSmithCharacterController();

	virtual void PlayerTick(float DeltaTime) override;

	void Select(AActor* Actor);
	void Deselect();

	void ReloadInventory();
	void ReloadAddInformation();
	bool AddItemToInventory(ABaseItem* AddingItem, bool Destroy = true);
	void ToggleInventoryUMG();

	void ShowViewportWidget(bool Show);

	UFUNCTION(BlueprintCallable)
	bool DropItemToWorld(UInventorySlot* InventorySlot, int32 Amount);

	UFUNCTION(BlueprintCallable)
	bool SwapItem(UInventorySlot* Slot1, UInventorySlot* Slot2);

	FORCEINLINE bool GetInventoryVisible() const { return bInventoryVisible; }
	FORCEINLINE bool GetViewportWidgetVisible() const { return bViewportWidgetVisible; }
	FORCEINLINE UCameraComponent* GetPawnCameraComponent() const{ if (ASpaceSmithCharacter* SpaceCharacter = Cast<ASpaceSmithCharacter>(GetPawn())) { return SpaceCharacter->GetCameraController(); } else { return nullptr; } }

	UInventorySlot* SelectSlot(int32 SlotIndex);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnAddItem(ABaseItem* AddingItem);

	UFUNCTION()
	void OnDropItem(UInventorySlot* Slot, FItemRow ItemRow, int32 Amount);

	UFUNCTION()
	void OnRemoveItem(UInventorySlot* Slot, int32 Amount);

	UFUNCTION()
	void OnSwapItem(UInventorySlot* Slot1, UInventorySlot* Slot2);

	UFUNCTION()
	void OnSetCapacity(int32 NewCapacity);

public:
	UPROPERTY(VisibleAnywhere)
	UInventoryComponent* Inventory;

	UPROPERTY(VisibleAnywhere)
	UInventoryComponent* QuickSlot;

private:
	UPROPERTY(VisibleAnywhere)
	class UPlayerMasterWidget* Widget;

	TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY(VisibleAnywhere)
	bool bInventoryVisible;

	UPROPERTY(VisibleAnywhere)
	bool bViewportWidgetVisible;

	UPROPERTY(VisibleAnywhere)
	int32 InventoryLimit = 30;

	UPROPERTY(VisibleAnywhere)
	int32 QuickSlotLimit = 10;

	UPROPERTY(VisibleAnywhere)
	UInventorySlot* CurrentSelectedQuickSlot;

	UPROPERTY(VisibleAnywhere)
	TArray<UInventoryAddInformation*> AddInformations;

	
};
