// Fill out your copyright notice in the Description page of Project Settings.


#include "StorageChest.h"
#include "InventoryWidget.h"
#include "SpaceSmithCharacterController.h"
#include "InventoryComponent.h"

AStorageChest::AStorageChest() : Super()
{
	static ConstructorHelpers::FClassFinder<UStorageInteractWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/SpaceSmith/UMG/StorageInteract'"));
	if (WidgetAsset.Succeeded())
	{
		InteractWidgetClass = WidgetAsset.Class;
	}
}

void AStorageChest::BeginPlay()
{
	Super::BeginPlay();

	bInteractWidgetOpen = false;
	InteractWidget = CreateWidget<UStorageInteractWidget>(GetWorld(), InteractWidgetClass);
	InteractWidget->MachineInventory->SetOwner(Inventory);
}

bool AStorageChest::Interact_Implementation(ASpaceSmithCharacterController* Controller)
{
	if (Super::Interact_Implementation(Controller))
	{
		if (Controller && Controller->Inventory)
		{
			if (bInteractWidgetOpen)
			{
				InteractWidget->RemoveFromViewport();
				Controller->ShowViewportWidget(false);
				bInteractWidgetOpen = false;
			}
			else
			{
				InteractWidget->AddToViewport(5);
				InteractWidget->PlayerInventory->SetOwner(Controller->Inventory);
				Controller->ShowViewportWidget(true);

				InteractWidget->MachineInventory->Clear();
				InteractWidget->PlayerInventory->Clear();

				const TArray<UInventorySlot*>& PlayerItems = Controller->Inventory->GetItems();
				for (auto & Item : PlayerItems)
				{
					InteractWidget->PlayerInventory->Add(Item);
				}

				const TArray<UInventorySlot*>& MachineItems = Inventory->GetItems();
				for (auto & Item : MachineItems)
				{
					InteractWidget->MachineInventory->Add(Item);
				}
				bInteractWidgetOpen = true;
			}
			return true;
		}
	}

	return false;
}
