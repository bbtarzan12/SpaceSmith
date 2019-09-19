// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSmithCharacterController.h"
#include "Public/BaseItem.h"
#include "Public/Widget/PlayerMasterWidget.h"
#include "PlayerInventoryWidget.h"
#include "SpaceSmithGameMode.h"
#include <WidgetBlueprintLibrary.h>
#include "SpaceSmithCharacter.h"

FItemRow* ASpaceSmithCharacterController::EmptyItemRow;

ASpaceSmithCharacterController::ASpaceSmithCharacterController()
{
	static ConstructorHelpers::FClassFinder<UPlayerMasterWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/SpaceSmith/UMG/PlayerMaster'"));
	if (WidgetAsset.Succeeded())
	{
		WidgetClass = WidgetAsset.Class;
	}
}

void ASpaceSmithCharacterController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());

	ASpaceSmithGameMode* GameMode = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode());
	EmptyItemRow = GameMode->ItemDataTable->FindRow<FItemRow>(TEXT("Empty"), FString("Can not Found Empty from FItemRow"));
	ensure(EmptyItemRow);

	for (int32 Num = 0; Num < InventoryLimit; Num++)
	{
		UInventorySlot* NewItem = NewObject<UInventorySlot>();
		NewItem->Amount = 0;
		NewItem->Row = *EmptyItemRow;
		Inventory.Add(NewItem);
	}

	for (int32 Num = 0; Num < QuickSlotLimit; Num++)
	{
		UInventorySlot* NewItem = NewObject<UInventorySlot>();
		NewItem->Amount = 0;
		NewItem->Row = *EmptyItemRow;
		QuickSlot.Add(NewItem);
	}

	Widget = CreateWidget<UPlayerMasterWidget>(GetWorld(), WidgetClass);
	if (ensure(Widget))
	{
		Widget->AddToViewport(9999);
		Widget->SetVisibility(ESlateVisibility::Visible);
		Widget->Inventory->SetVisibility(ESlateVisibility::Hidden);
		ReloadInventory();
	}

	CurrentSelectedSlot = nullptr;
}


void ASpaceSmithCharacterController::ReloadInventory()
{
	Widget->Inventory->ClearSlots();
	Widget->QuickBar->ClearSlots();

	for (auto & Item : Inventory)
	{
		Widget->Inventory->AddSlot(Item);
	}

	for (auto& Item : QuickSlot)
	{
		Widget->QuickBar->AddSlot(Item);
	}
}

bool ASpaceSmithCharacterController::AddItemToInventory(ABaseItem* AddingItem, bool Destroy)
{
	UInventorySlot* StoredSlot = nullptr;
	
	const FItemRow& ItemData = AddingItem->ItemData;

	if (ItemData.bStack)
	{
		for (auto & Slot : Inventory)
		{
			if (Slot->Row == ItemData)
			{
				StoredSlot = Slot;
				break;
			}
		}
	}

	if (StoredSlot == nullptr)
	{
		for (auto & Slot : Inventory)
		{
			if (Slot->Row.ItemID == 0)
			{
				StoredSlot = Slot;
				break;
			}
		}
	}

	if (StoredSlot)
	{
		if (StoredSlot->Row.ItemID == 0)
		{
			StoredSlot->Row = ItemData;
			StoredSlot->Amount = 1;
		}
		else
		{
			StoredSlot->Amount++;
		}
	}

	if (Destroy)
	{
		AddingItem->Destroy();
	}

	ReloadInventory();
	return true;
}

void ASpaceSmithCharacterController::ToggleInventoryUMG()
{
	if (Widget->Inventory->IsVisible())
	{
		Widget->Inventory->SetVisibility(ESlateVisibility::Hidden);
		bInventoryVisible = false;
		bShowMouseCursor = false;
		bEnableClickEvents = false;
		bEnableMouseOverEvents = false;
		SetInputMode(FInputModeGameOnly());
	}
	else
	{
		Widget->Inventory->SetVisibility(ESlateVisibility::Visible);
		bInventoryVisible = true;
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
		ReloadInventory();
		SetInputMode(FInputModeGameAndUI());
	}
}

bool ASpaceSmithCharacterController::DropItemToWorld(UInventorySlot* Slot, int32 Amount)
{
	if (Slot == nullptr)
		return false;

	if (Slot->Amount < Amount)
		return false;

	Slot->Amount -= Amount;

	FVector PawnLocation = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 50.0f;
	FRotator PawnRotator = FQuat::Identity.Rotator();

	for (int32 Num = 0; Num < Amount; Num++)
	{
		if (ABaseItem* ItemActor = GetWorld()->SpawnActor<ABaseItem>(ABaseItem::StaticClass(), PawnLocation + GetPawn()->GetActorUpVector() * Num * 25.0f, PawnRotator))
		{
			ItemActor->InitializeItem(Slot->Row);
		}
	}

	if (Slot->Amount <= 0)
	{
		Slot->Row = *EmptyItemRow;
		Slot->Amount = 0;
	}

	if (Slot == CurrentSelectedSlot)
	{
		ASpaceSmithCharacter* SmithCharacter = Cast<ASpaceSmithCharacter>(GetPawn());
		SmithCharacter->Slot(CurrentSelectedSlot);
	}

	ReloadInventory();
	return true;
}

bool ASpaceSmithCharacterController::SwapItem(UInventorySlot* Slot1, UInventorySlot* Slot2)
{
	FItemRow TempRow;
	int32 TempAmount;

	TempRow = Slot1->Row;
	TempAmount = Slot1->Amount;

	Slot1->Row = Slot2->Row;
	Slot1->Amount = Slot2->Amount;

	Slot2->Row = TempRow;
	Slot2->Amount = TempAmount;

	// 놓은 곳이 슬롯이라면
	if (QuickSlot.Contains(Slot1))
	{
		// 근데 선택된 슬롯에 놨다면
		if (CurrentSelectedSlot == Slot1)
		{
			ASpaceSmithCharacter* SmithCharacter = Cast<ASpaceSmithCharacter>(GetPawn());
			SmithCharacter->Slot(Slot1);
		}
	}

	// 선택된 슬롯이 있는데 이 슬롯이 이제 비어있다면
	if (CurrentSelectedSlot && CurrentSelectedSlot->Row.ItemID == 0)
	{
		ASpaceSmithCharacter* SmithCharacter = Cast<ASpaceSmithCharacter>(GetPawn());
		SmithCharacter->Slot(CurrentSelectedSlot);
	}

	ReloadInventory();
	return true;
}

UInventorySlot* ASpaceSmithCharacterController::SelectSlot(int32 SlotIndex)
{
	ensureMsgf(SlotIndex >= 0 && SlotIndex < QuickSlotLimit, TEXT("SlotIndex : %d, QuicSlotLimit : %d"), SlotIndex, QuickSlotLimit);

	if (CurrentSelectedSlot == QuickSlot[SlotIndex])
	{
		CurrentSelectedSlot->bSelected = false;
		CurrentSelectedSlot = nullptr;
		return nullptr;
	}

	if (CurrentSelectedSlot)
	{
		CurrentSelectedSlot->bSelected = false;
	}

	CurrentSelectedSlot = QuickSlot[SlotIndex];
	CurrentSelectedSlot->bSelected = true;
	ReloadInventory();
	return CurrentSelectedSlot;
}
