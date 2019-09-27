// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Public/BaseItem.h"
#include "SpaceSmithGameMode.h"

FItemRow UInventoryComponent::EmptyItemRow;

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}

void UInventoryComponent::SetName(FText NewInventoryName)
{
	InventoryName = NewInventoryName;
	OnAnyChange.Broadcast();
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ASpaceSmithGameMode* GameMode = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode());
	EmptyItemRow = *GameMode->ItemDataTable->FindRow<FItemRow>(TEXT("Empty"), FString("Can not Found Empty from FItemRow"));

	SetCapacity(1);
}

bool UInventoryComponent::AddItem(ABaseItem* AddingItem, bool Destroy /*= true*/)
{
	const FItemRow& ItemData = AddingItem->Data;

	if (AddItem(ItemData))
	{
		if (Destroy)
		{
			AddingItem->Destroy();
		}

		OnAddItem.Broadcast(AddingItem);
		OnAnyChange.Broadcast();
		return true;
	}

	return false;
}

bool UInventoryComponent::AddItem(FItemRow ItemData)
{
	UInventorySlot* StoredSlot = nullptr;

	// 스택을 쌓을 수 있다면
	if (ItemData.bStack)
	{
		// 같은 종류의 아이템을 찾는다
		for (auto & Slot : Inventory)
		{
			if (Slot->Row == ItemData)
			{
				StoredSlot = Slot;
				break;
			}
		}
	}

	// 같은 종류의 아이템이 없다면
	if (StoredSlot == nullptr)
	{
		// 빈 아이템 슬롯을 찾는다
		for (auto & Slot : Inventory)
		{
			if (Slot->Row.ItemID == 0)
			{
				StoredSlot = Slot;
				break;
			}
		}
	}

	// 스택을 쌓을 수 있는 아이템 슬롯을 찾았거나,
	// 빈 아이템 슬롯을 찾았다면 성공
	if (StoredSlot)
	{
		if (StoredSlot->Row.ItemID == 0)
		{
			StoredSlot->Row = ItemData;
			StoredSlot->Amount = 1;
			StoredSlot->Inventory = this;
		}
		else
		{
			StoredSlot->Amount++;
			StoredSlot->Inventory = this;
		}
	}
	else // 실패
	{
		return false;
	}

	return true;
}

void UInventoryComponent::DropItem(UInventorySlot* Slot, int32 Amount)
{
	if (Slot == nullptr)
		return;

	if (Slot->Amount < Amount)
		return;

	Slot->Amount -= Amount;
	FItemRow ItemRow = Slot->Row;

	if (Slot->Amount <= 0)
	{
		Slot->Amount = 0;
		Slot->Row = EmptyItemRow;
	}

	OnDropItem.Broadcast(Slot, ItemRow, Amount);
	OnAnyChange.Broadcast();
}

void UInventoryComponent::RemoveItem(UInventorySlot* Slot, int32 Amount)
{
	if (Slot == nullptr)
		return;

	if (Slot->Amount < Amount)
		return;

	Slot->Amount -= Amount;
	FItemRow ItemRow = Slot->Row;

	if (Slot->Amount <= 0)
	{
		Slot->Amount = 0;
		Slot->Row = EmptyItemRow;
	}

	OnRemoveItem.Broadcast(Slot, Amount);
	OnAnyChange.Broadcast();
}

void UInventoryComponent::RemoveItem(FItemRow Row, int32 Amount)
{
	if (Contains(Row, Amount))
	{
		int32 RemainItems = Amount;
		while (RemainItems > 0)
		{
			if (UInventorySlot* Slot = FindSlot(Row))
			{
				if (Slot->Amount <= RemainItems)
				{
					RemainItems -= Slot->Amount;
					RemoveItem(Slot, Slot->Amount);
				}
				else
				{
					RemoveItem(Slot, RemainItems);
					RemainItems -= RemainItems;
				}
			}
			else
			{
				break;
			}
		}
	}
}

void UInventoryComponent::SwapItem(UInventorySlot* CurrentSlot, UInventorySlot* PayloadSlot)
{
	FItemRow TempRow;
	int32 TempAmount;

	if (CurrentSlot->Row == PayloadSlot->Row && CurrentSlot->Row.bStack)
	{
		CurrentSlot->Amount += PayloadSlot->Amount;
		PayloadSlot->Amount = 0;
		PayloadSlot->Row = EmptyItemRow;
	}
	else
	{
		TempRow = CurrentSlot->Row;
		TempAmount = CurrentSlot->Amount;

		CurrentSlot->Row = PayloadSlot->Row;
		CurrentSlot->Amount = PayloadSlot->Amount;

		PayloadSlot->Row = TempRow;
		PayloadSlot->Amount = TempAmount;
	}

	CurrentSlot->Inventory->OnSwapItem.Broadcast(CurrentSlot, PayloadSlot);
	CurrentSlot->Inventory->OnAnyChange.Broadcast();

	PayloadSlot->Inventory->OnSwapItem.Broadcast(PayloadSlot, CurrentSlot);
	PayloadSlot->Inventory->OnAnyChange.Broadcast();
}

void UInventoryComponent::SetCapacity(int32 NewCapacity)
{
	int32 NumNewSlots = NewCapacity - Inventory.Num();

	if (NumNewSlots < 0)
		return;

	for (int32 Num = 0; Num < NumNewSlots; Num++)
	{
		UInventorySlot* NewItem = NewObject<UInventorySlot>();
		NewItem->Amount = 0;
		NewItem->Row = EmptyItemRow;
		NewItem->Inventory = this;
		Inventory.Add(NewItem);
	}

	OnSetCapacity.Broadcast(NewCapacity);
	OnAnyChange.Broadcast();
}

bool UInventoryComponent::Contains(UInventorySlot* Slot)
{
	return Inventory.Contains(Slot);
}

bool UInventoryComponent::Contains(ABaseItem* Item)
{
	return Inventory.ContainsByPredicate([&](const UInventorySlot* Slot) { return Slot->Row == Item->Data; });
}

bool UInventoryComponent::Contains(FItemRow Row, int32 Amount)
{
	int32 NumItems = 0;
	for (auto & Slot : Inventory)
	{
		if (Slot->Row == Row)
		{
			NumItems += Slot->Amount;
		}

		if (NumItems >= Amount)
			return true;
	}

	return false;
}

bool UInventoryComponent::CanAddItems(TArray<FItemRow>& OutItems)
{
	int32 NumItems = OutItems.Num();
	for (auto & Item : OutItems)
	{
		if (Item.bStack && Contains(Item, 1))
		{
			NumItems -= 1;
		}
	}

	return NumItems <= GetRemainSlots();
}

int32 UInventoryComponent::Count(FItemRow Row)
{
	int32 NumItems = 0;
	for (auto & Slot : Inventory)
	{
		if (Slot->Row == Row)
		{
			NumItems += Slot->Amount;
		}
	}

	return NumItems;
}

int32 UInventoryComponent::GetRemainSlots()
{
	int32 NumRemain = 0;
	for (auto & Slot : Inventory)
	{
		if (Slot->Row == EmptyItemRow)
		{
			NumRemain++;
		}
	}
	return NumRemain;
}

UInventorySlot* UInventoryComponent::FindSlot(FItemRow Row)
{
	if (UInventorySlot** InventorySlot = Inventory.FindByPredicate([&](const UInventorySlot* Slot) { return Slot->Row == Row; }))
	{
		return *InventorySlot;
	}

	return nullptr;
}
