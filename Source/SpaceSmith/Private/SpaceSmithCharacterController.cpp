// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSmithCharacterController.h"
#include "Public/BaseItem.h"
#include "Public/Widget/PlayerMasterWidget.h"
#include "PlayerInventoryWidget.h"
#include "SpaceSmithGameMode.h"
#include <WidgetBlueprintLibrary.h>
#include "SpaceSmithCharacter.h"
#include "Public/Component/InventoryComponent.h"
#include "Public/Widget/KeyInformationWidget.h"
#include <GameFramework/PlayerInput.h>

ASpaceSmithCharacterController::ASpaceSmithCharacterController()
{
	static ConstructorHelpers::FClassFinder<UPlayerMasterWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/SpaceSmith/UMG/PlayerMaster'"));
	if (WidgetAsset.Succeeded())
	{
		WidgetClass = WidgetAsset.Class;
	}

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	QuickSlot = CreateDefaultSubobject<UInventoryComponent>(TEXT("QuickSlot"));
}

void ASpaceSmithCharacterController::Select(AActor* Actor)
{
	Widget->KeyInformation->Clear();

	UClass* ActorClass = Actor->GetClass();
	if (ActorClass->ImplementsInterface(UInteract::StaticClass()))
	{
		const TArray<FInputActionKeyMapping>& ActionKeyMappings = PlayerInput->GetKeysForAction(TEXT("Interact"));
		if (ActionKeyMappings.Num() > 0)
		{
			UKeyInformation* KeyInformation = NewObject<UKeyInformation>(this);
			KeyInformation->Key = FText::Format(FTextFormat(FText::FromString("[{0}]")), FText::FromString(ActionKeyMappings[0].Key.ToString()));
			KeyInformation->KeyInformation = FText::FromStringTable("/Game/SpaceSmith/Data/StringTable/KeyInformation", (ActionKeyMappings[0].ActionName.ToString()));
			Widget->KeyInformation->Add(KeyInformation);
		}
	}

	if (ActorClass->ImplementsInterface(UPick::StaticClass()))
	{
		const TArray<FInputActionKeyMapping>& ActionKeyMappings = PlayerInput->GetKeysForAction(TEXT("Hold"));
		if (ActionKeyMappings.Num() > 0)
		{
			UKeyInformation* KeyInformation = NewObject<UKeyInformation>(this);
			KeyInformation->Key = FText::Format(FTextFormat(FText::FromString("[{0}]")), FText::FromString(ActionKeyMappings[0].Key.ToString()));
			KeyInformation->KeyInformation = FText::FromStringTable("/Game/SpaceSmith/Data/StringTable/KeyInformation", (ActionKeyMappings[0].ActionName.ToString()));
			Widget->KeyInformation->Add(KeyInformation);
		}
	}
	Widget->KeyInformation->SetVisibility(ESlateVisibility::Visible);
}

void ASpaceSmithCharacterController::Deselect()
{
	Widget->KeyInformation->Clear();
	Widget->KeyInformation->SetVisibility(ESlateVisibility::Hidden);
}

void ASpaceSmithCharacterController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());

	Widget = CreateWidget<UPlayerMasterWidget>(GetWorld(), WidgetClass);
	if (ensure(Widget))
	{
		Widget->AddToViewport(9999);
		Widget->SetVisibility(ESlateVisibility::Visible);
		Widget->Inventory->SetVisibility(ESlateVisibility::Hidden);
	}

	Inventory->OnAddItem.AddDynamic(this, &ASpaceSmithCharacterController::OnAddItem);
	Inventory->OnDropItem.AddDynamic(this, &ASpaceSmithCharacterController::OnDropItem);
	Inventory->OnSwapItem.AddDynamic(this, &ASpaceSmithCharacterController::OnSwapItem);
	Inventory->OnSetCapacity.AddDynamic(this, &ASpaceSmithCharacterController::OnSetCapacity);

	QuickSlot->OnAddItem.AddDynamic(this, &ASpaceSmithCharacterController::OnAddItem);
	QuickSlot->OnDropItem.AddDynamic(this, &ASpaceSmithCharacterController::OnDropItem);
	QuickSlot->OnSwapItem.AddDynamic(this, &ASpaceSmithCharacterController::OnSwapItem);
	QuickSlot->OnSetCapacity.AddDynamic(this, &ASpaceSmithCharacterController::OnSetCapacity);

	Inventory->SetCapacity(InventoryLimit);
	QuickSlot->SetCapacity(QuickSlotLimit);

	ReloadInventory();
	CurrentSelectedQuickSlot = nullptr;
}

void ASpaceSmithCharacterController::OnAddItem(ABaseItem* AddingItem)
{
	ReloadInventory();
}

void ASpaceSmithCharacterController::OnDropItem(UInventorySlot* Slot, FItemRow ItemRow, int32 Amount)
{
	if (Slot == CurrentSelectedQuickSlot)
	{
		ASpaceSmithCharacter* SmithCharacter = Cast<ASpaceSmithCharacter>(GetPawn());
		SmithCharacter->Slot(CurrentSelectedQuickSlot);
	}

	FVector PawnLocation = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 50.0f;
	FRotator PawnRotator = FQuat::Identity.Rotator();

	for (int32 Num = 0; Num < Amount; Num++)
	{
		if (ABaseItem* ItemActor = GetWorld()->SpawnActor<ABaseItem>(ABaseItem::StaticClass(), PawnLocation + GetPawn()->GetActorUpVector() * Num * 25.0f, PawnRotator))
		{
			ItemActor->Initialize(ItemRow);
		}
	}

	if (Slot == CurrentSelectedQuickSlot)
	{
		ASpaceSmithCharacter* SmithCharacter = Cast<ASpaceSmithCharacter>(GetPawn());
		SmithCharacter->Slot(CurrentSelectedQuickSlot);
	}

	ReloadInventory();
}

void ASpaceSmithCharacterController::OnSwapItem(UInventorySlot* Slot1, UInventorySlot* Slot2)
{
	// 놓은 곳이 슬롯이라면
	if (QuickSlot->Contains(Slot1))
	{
		// 근데 선택된 슬롯에 놨다면
		if (CurrentSelectedQuickSlot == Slot1)
		{
			ASpaceSmithCharacter* SmithCharacter = Cast<ASpaceSmithCharacter>(GetPawn());
			SmithCharacter->Slot(Slot1);
		}
	}

	// 선택된 슬롯이 있는데 이 슬롯이 이제 비어있다면
	if (CurrentSelectedQuickSlot && CurrentSelectedQuickSlot->Row.ItemID == 0)
	{
		ASpaceSmithCharacter* SmithCharacter = Cast<ASpaceSmithCharacter>(GetPawn());
		SmithCharacter->Slot(CurrentSelectedQuickSlot);
	}

	ReloadInventory();
}

void ASpaceSmithCharacterController::OnSetCapacity(int32 NewCapacity)
{
	ReloadInventory();
}

void ASpaceSmithCharacterController::ReloadInventory()
{
	Widget->Inventory->Clear();
	Widget->QuickBar->Clear();

	for (auto & Item : Inventory->GetItems())
	{
		Widget->Inventory->Add(Item);
	}

	for (auto& Item : QuickSlot->GetItems())
	{
		Widget->QuickBar->Add(Item);
	}
}

bool ASpaceSmithCharacterController::AddItemToInventory(ABaseItem* AddingItem, bool Destroy)
{
	Inventory->AddItem(AddingItem, Destroy);
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
	if (Slot->Inventory)
	{
		Slot->Inventory->DropItem(Slot, Amount);
	}
	return true;
}

bool ASpaceSmithCharacterController::SwapItem(UInventorySlot* Slot1, UInventorySlot* Slot2)
{
	Inventory->SwapItem(Slot1, Slot2);
	return true;
}

UInventorySlot* ASpaceSmithCharacterController::SelectSlot(int32 SlotIndex)
{
	ensureMsgf(SlotIndex >= 0 && SlotIndex < QuickSlotLimit, TEXT("SlotIndex : %d, QuicSlotLimit : %d"), SlotIndex, QuickSlotLimit);

	const TArray<UInventorySlot*>& QuickSlotItems = QuickSlot->GetItems();

	if (CurrentSelectedQuickSlot == QuickSlotItems[SlotIndex])
	{
		CurrentSelectedQuickSlot->bSelected = false;
		CurrentSelectedQuickSlot = nullptr;
		return nullptr;
	}

	if (CurrentSelectedQuickSlot)
	{
		CurrentSelectedQuickSlot->bSelected = false;
	}

	CurrentSelectedQuickSlot = QuickSlotItems[SlotIndex];
	CurrentSelectedQuickSlot->bSelected = true;

	ReloadInventory();
	return CurrentSelectedQuickSlot;
}
