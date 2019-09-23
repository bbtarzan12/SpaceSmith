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
		if (bInteractWidgetOpen)
		{
			InteractWidget->RemoveFromViewport();
			Controller->EnableGameInputModeAndMouse(true);
			bInteractWidgetOpen = false;
		}
		else
		{
			InteractWidget->AddToViewport(5);
			InteractWidget->PlayerInventory->SetOwner(Controller->Inventory);
			Controller->EnableGameInputModeAndMouse(false);

			InteractWidget->MachineInventory->Clear();
			InteractWidget->PlayerInventory->Clear();

			for (auto & Item : Controller->Inventory->GetItems())
			{
				InteractWidget->PlayerInventory->Add(Item);
			}

			for (auto & Item : Inventory->GetItems())
			{
				InteractWidget->MachineInventory->Add(Item);
			}
			bInteractWidgetOpen = true;
		}
		return true;
	}

	return false;
}
