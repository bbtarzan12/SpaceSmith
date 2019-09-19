// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSmithCharacterController.h"
#include "Public/BaseItem.h"
#include "Public/Widget/PlayerMasterWidget.h"
#include "PlayerInventoryWidget.h"

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

	Widget = CreateWidget<UPlayerMasterWidget>(GetWorld(), WidgetClass);
	if (ensure(Widget))
	{
		Widget->AddToViewport(9999);
		Widget->SetVisibility(ESlateVisibility::Visible);
		Widget->Inventory->SetVisibility(ESlateVisibility::Hidden);
		ReloadInventory();
	}
}


void ASpaceSmithCharacterController::ReloadInventory()
{
	Widget->Inventory->ClearTiles();

	for (auto & Item : Inventory)
	{
		Widget->Inventory->AddItem(Item);
	}
	Widget->Inventory->FillEmptyTiles(InventoryLimit - Inventory.Num());
}

bool ASpaceSmithCharacterController::AddItemToInventory(ABaseItem* AddingItem, bool Destroy)
{
	UInventoryItem* StoredItem = nullptr;
	
	const FItemRow& ItemData = AddingItem->ItemData;

	if (ItemData.bStack)
	{
		for (auto & Item : Inventory)
		{
			if (Item->Row.ItemID == 0 || Item->Row == ItemData)
			{
				StoredItem = Item;
				break;
			}
		}
	}

	if (StoredItem)
	{
		if (StoredItem->Row.ItemID == 0)
		{
			StoredItem->Row = ItemData;
			StoredItem->Amount = 1;
		}
		else
		{
			StoredItem->Amount++;
		}
	}
	else
	{
		UInventoryItem* NewItem = NewObject<UInventoryItem>();
		NewItem->Amount = 1;
		NewItem->Row = ItemData;
		Inventory.Add(NewItem);
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
	}
	else
	{
		Widget->Inventory->SetVisibility(ESlateVisibility::Visible);
		bInventoryVisible = true;
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
		ReloadInventory();
	}
}

bool ASpaceSmithCharacterController::DropItemToWorld(const FItemRow& ItemRow, int32 Amount)
{
	UInventoryItem* StoredItem = nullptr;

	for (auto & Item : Inventory)
	{
		if (Item->Row == ItemRow)
		{
			StoredItem = Item;
			break;
		}
	}

	if (StoredItem == nullptr)
	{
		return false;
	}

	if (StoredItem->Amount < Amount)
	{
		return false;
	}

	StoredItem->Amount -= Amount;

	if (StoredItem->Amount <= 0)
	{
		Inventory.Remove(StoredItem);
	}

	FVector PawnLocation = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 50.0f;
	FRotator PawnRotator = FQuat::Identity.Rotator();

	for (int32 Num = 0; Num < Amount; Num++)
	{
		if (ABaseItem* ItemActor = GetWorld()->SpawnActor<ABaseItem>(ABaseItem::StaticClass(), PawnLocation + GetPawn()->GetActorUpVector() * Num * 25.0f, PawnRotator))
		{
			ItemActor->InitializeItem(StoredItem->Row);
		}
	}

	ReloadInventory();
	return true;
}