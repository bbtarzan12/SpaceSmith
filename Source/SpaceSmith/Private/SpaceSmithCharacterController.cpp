// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSmithCharacterController.h"
#include "Public/BaseItem.h"
#include "Public/Widget/PlayerMasterWidget.h"
#include "PlayerInventoryWidget.h"
#include "InventoryWidget.h"
#include "InventoryWidget.h"
#include "SpaceSmithGameMode.h"
#include <WidgetBlueprintLibrary.h>
#include "SpaceSmithCharacter.h"
#include "Public/Component/InventoryComponent.h"
#include "Public/Widget/KeyInformationWidget.h"
#include <GameFramework/PlayerInput.h>
#include "Debug/SpaceSmithCheatManager.h"

ASpaceSmithCharacterController::ASpaceSmithCharacterController()
{
	static ConstructorHelpers::FClassFinder<UPlayerMasterWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/SpaceSmith/UMG/WBP_PlayerMaster'"));
	if (WidgetAsset.Succeeded())
	{
		WidgetClass = WidgetAsset.Class;
	}

	CheatClass = USpaceSmithCheatManager::StaticClass();
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	QuickSlot = CreateDefaultSubobject<UInventoryComponent>(TEXT("QuickSlot"));
}

void ASpaceSmithCharacterController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());

	Widget = CreateWidget<UPlayerMasterWidget>(GetWorld(), WidgetClass);
	if (ensure(Widget))
	{
		Widget->SetController(this);
		Widget->AddToViewport(0);
		Widget->SetVisibility(ESlateVisibility::Visible);
		Widget->Inventory->SetOwner(Inventory);
		Widget->QuickBar->SetOwner(QuickSlot);
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
	Inventory->SetName(FText::FromString("Player"));
	QuickSlot->SetCapacity(QuickSlotLimit);

	ReloadInventory();
	CurrentSelectedQuickSlot = nullptr;
	bInventoryVisible = false;
}

void ASpaceSmithCharacterController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	float CurrentTime = GetWorld()->GetTimeSeconds();
	int32 NumOfAddInformations = AddInformations.Num();
	AddInformations.RemoveAll([&](const UInventoryAddInformation* AddInformation) 
	{
		return CurrentTime - AddInformation->LastAddTime > 2.0f;
	});

	if (NumOfAddInformations != AddInformations.Num())
	{
		ReloadAddInformation();
	}
}

void ASpaceSmithCharacterController::Select(AActor* Actor)
{
	Widget->KeyInformation->Clear();

	if (Actor)
	{
		UClass* ActorClass = Actor->GetClass();
		if (ActorClass->ImplementsInterface(UInteract::StaticClass()))
		{
			const TArray<FInputActionKeyMapping>& ActionKeyMappings = PlayerInput->GetKeysForAction(TEXT("Interact"));
			if (ActionKeyMappings.Num() > 0)
			{
				UKeyInformation* KeyInformation = NewObject<UKeyInformation>(this);
				KeyInformation->Key = FText::Format(FTextFormat(FText::FromString("[{0}]")), FText::FromString(ActionKeyMappings[0].Key.ToString()));
				KeyInformation->KeyInformation = IInteract::Execute_GetInteractInformationText(Actor);

				if (!KeyInformation->KeyInformation.IsEmptyOrWhitespace())
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
				KeyInformation->KeyInformation = IPick::Execute_GetPickInformationText(Actor);

				if (!KeyInformation->KeyInformation.IsEmptyOrWhitespace())
					Widget->KeyInformation->Add(KeyInformation);
			}
		}

		if (ActorClass->ImplementsInterface(UAction::StaticClass()))
		{
			const TArray<FInputActionKeyMapping>& ActionKeyMappings = PlayerInput->GetKeysForAction(TEXT("Action"));
			if (ActionKeyMappings.Num() > 0)
			{
				UKeyInformation* KeyInformation = NewObject<UKeyInformation>(this);
				KeyInformation->Key = FText::Format(FTextFormat(FText::FromString("[{0}]")), FText::FromString(ActionKeyMappings[0].Key.ToString()));
				KeyInformation->KeyInformation = IAction::Execute_GetActionInformationText(Actor);

				if (!KeyInformation->KeyInformation.IsEmptyOrWhitespace())
					Widget->KeyInformation->Add(KeyInformation);
			}
		}
	}
}

void ASpaceSmithCharacterController::Deselect()
{
	Widget->KeyInformation->Clear();
}

void ASpaceSmithCharacterController::OnAddItem(ABaseItem* AddingItem)
{
	AddingItem->SetOwnerController(this);

	if (UInventoryAddInformation** AddInformation = AddInformations.FindByPredicate([&](UInventoryAddInformation* Information) { return Information->Row == AddingItem->Data; }))
	{
		(*AddInformation)->Amount += 1;
		(*AddInformation)->LastAddTime = GetWorld()->GetTimeSeconds();
	}
	else
	{
		UInventoryAddInformation* NewInformation = NewObject<UInventoryAddInformation>();
		NewInformation->Row = AddingItem->Data;
		NewInformation->Amount = 1;
		NewInformation->LastAddTime = GetWorld()->GetTimeSeconds();
		AddInformations.Add(NewInformation);
	}

	ReloadAddInformation();
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
		PawnLocation.Z += Num * 25.0f;
		if (ABaseItem* ItemActor = GetWorld()->SpawnActor<ABaseItem>(ItemRow.Class, PawnLocation, PawnRotator))
		{
			ItemActor->Initialize(ItemRow);
			ItemActor->SetOwnerController(nullptr);
		}
	}
	ReloadInventory();
}

void ASpaceSmithCharacterController::OnRemoveItem(UInventorySlot* Slot, int32 Amount)
{
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

void ASpaceSmithCharacterController::ReloadAddInformation()
{
	Widget->InventoryAddInformation->Clear();

	for (auto & AddInformation : AddInformations)
	{
		Widget->InventoryAddInformation->Add(AddInformation);
	}

}

bool ASpaceSmithCharacterController::AddItemToInventory(ABaseItem* AddingItem, bool Destroy)
{
	if (AddingItem->Data.bStack)
	{
		if (Inventory->Contains(AddingItem))
		{
			if (Inventory->AddItem(AddingItem, Destroy))
			{
				return true;
			}
		}

		if (QuickSlot->Contains(AddingItem))
		{
			if (QuickSlot->AddItem(AddingItem, Destroy))
			{
				return true;
			}
		}
	}

	if (!Inventory->AddItem(AddingItem, Destroy))
	{
		if (!QuickSlot->AddItem(AddingItem, Destroy))
		{
			return false;
		}
	}
	return true;
}

void ASpaceSmithCharacterController::ToggleInventoryUMG()
{
	if (Widget->Inventory->IsVisible())
	{
		bInventoryVisible = false;
		ShowViewportWidget(false);
	}
	else
	{
		bInventoryVisible = true;
		ReloadInventory();
		ShowViewportWidget(true);
	}
}

void ASpaceSmithCharacterController::ShowViewportWidget(bool Show)
{
	if (Show)
	{
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
		bViewportWidgetVisible = true;
		SetInputMode(FInputModeGameAndUI());
	}
	else
	{
		bShowMouseCursor = false;
		bEnableClickEvents = false;
		bEnableMouseOverEvents = false;
		bViewportWidgetVisible = false;
		SetInputMode(FInputModeGameOnly());
	}
}

bool ASpaceSmithCharacterController::DropItemToWorld(UInventorySlot* Slot, int32 Amount)
{
	if (Inventory)
	{
		Inventory->DropItem(Slot, Amount);
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
