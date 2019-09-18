// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSmithCharacterController.h"
#include "Public/Widget/PlayerInventoryWidget.h"

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

bool ASpaceSmithCharacterController::AddItemToInventory(const FItemRow& ItemRow)
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

	if (StoredItem)
	{
		StoredItem->Amount++;
	}
	else
	{
		UInventoryItem* NewItem = NewObject<UInventoryItem>();
		NewItem->Amount = 1;
		NewItem->Row = ItemRow;
		Inventory.Add(NewItem);
	}

	ReloadInventory();
	return true;
}

void ASpaceSmithCharacterController::ToggleInventoryUMG()
{
	if (InventoryWidget->IsVisible())
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
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
