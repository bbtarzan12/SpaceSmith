// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../DataTable/ItemDataTable.h"
#include "Delegates/Delegate.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryOnAnyChangeSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryOnAddSignature, class ABaseItem*, AddingItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInventoryOnDropSignature, class UInventorySlot*, Slot, FItemRow, ItemRow, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryOnSwapSignature, class UInventorySlot*, Slot1, class UInventorySlot*, Slot2);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryOnSetCapacitySignature, int32, NewCapacity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryOnRemoveSignature, class UInventorySlot*, Slot, int32, Amount);


class ABaseItem;

UCLASS(BlueprintType)
class UInventorySlot : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemRow Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSelected;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* Inventory;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESMITH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	void SetName(FText NewInventoryName);
	bool AddItem(ABaseItem* AddingItem, bool Destroy = true);
	bool AddItem(FItemRow AddingRow);
	void DropItem(UInventorySlot* Slot, int32 Amount);
	void RemoveItem(UInventorySlot* Slot, int32 Amount);
	void RemoveItem(FItemRow Row, int32 Amount);
	void SetCapacity(int32 NewCapacity);
	bool Contains(ABaseItem* Item);
	bool Contains(UInventorySlot* Slot);
	bool Contains(FItemRow Row, int32 Amount);
	bool CanAddItems(TArray<FItemRow>& OutItems);
	int32 Count(FItemRow Row);
	int32 GetRemainSlots();
	UInventorySlot* FindSlot(FItemRow Row);

	static void SwapItem(UInventorySlot* CurrentSlot, UInventorySlot* PlayloadSlot);


	FORCEINLINE const TArray<UInventorySlot*>& GetItems() const { return Inventory; };
	FORCEINLINE const FText& GetInventoryName() const { return InventoryName; }

	UPROPERTY(BlueprintAssignable)
	FInventoryOnAnyChangeSignature OnAnyChange;

	UPROPERTY(BlueprintAssignable)
	FInventoryOnAddSignature OnAddItem;

	UPROPERTY(BlueprintAssignable)
	FInventoryOnDropSignature OnDropItem;

	UPROPERTY(BlueprintAssignable)
	FInventoryOnRemoveSignature OnRemoveItem;

	UPROPERTY(BlueprintAssignable)
	FInventoryOnSwapSignature OnSwapItem;

	UPROPERTY(BlueprintAssignable)
	FInventoryOnSetCapacitySignature OnSetCapacity;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TArray<UInventorySlot*> Inventory;

	UPROPERTY(VisibleAnywhere)
	FText InventoryName;

	static FItemRow EmptyItemRow;

};
