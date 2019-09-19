// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSmithCharacterController.h"
#include "Public/Widget/PlayerInventoryWidget.h"
#include "Public/BaseItem.h"

ASpaceSmithCharacterController::ASpaceSmithCharacterController()
{
	static ConstructorHelpers::FClassFinder<UPlayerInventoryWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/SpaceSmith/UMG/PlayerInventory'"));
	if (WidgetAsset.Succeeded())
	{
		InventoryWidgetClass = WidgetAsset.Class;
	}
}

void ASpaceSmithCharacterController::ReloadInventory()
{
	InventoryWidget->ClearTiles();

	for (auto & Item : Inventory)
	{
		InventoryWidget->AddItem(Item);
	}
}

bool ASpaceSmithCharacterController::AddItemToInventory(ABaseItem* AddingItem, bool Destroy)
{
	UInventoryItem* StoredItem = nullptr;
	
	const FItemRow& ItemData = AddingItem->ItemData;

	if (ItemData.bStack)
	{
		for (auto & Item : Inventory)
		{
			if (Item->Row == ItemData)
			{
				StoredItem = Item;
				break;
			}
		}
	}

	if (StoredItem)
	{
		StoredItem->Amount++;
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
	if (InventoryWidget->IsVisible())
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		bInventoryVisible = false;
		bShowMouseCursor = false;
		bEnableClickEvents = false;
		bEnableMouseOverEvents = false;
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		bInventoryVisible = true;
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
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

void ASpaceSmithCharacterController::BeginPlay()
{
	Super::BeginPlay();

	InventoryWidget = CreateWidget<UPlayerInventoryWidget>(GetWorld(), InventoryWidgetClass);
	if (ensure(InventoryWidget))
	{
		InventoryWidget->AddToViewport(9999);
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
