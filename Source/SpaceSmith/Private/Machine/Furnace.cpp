// Fill out your copyright notice in the Description page of Project Settings.


#include "Furnace.h"
#include "InventoryWidget.h"
#include "SpaceSmithCharacterController.h"
#include "InventoryComponent.h"
#include "FurnaceInteractWidget.h"

AFurnace::AFurnace()
{
	static ConstructorHelpers::FClassFinder<UFurnaceInteractWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/SpaceSmith/UMG/Machine/WBP_FurnaceInteract'"));
	if (WidgetAsset.Succeeded())
	{
		InteractWidgetClass = WidgetAsset.Class;
	}

	bMachineTickable = true;
}

void AFurnace::BeginPlay()
{
	Super::BeginPlay();

	bInteractWidgetOpen = false;
	InteractWidget = CreateWidget<UFurnaceInteractWidget>(GetWorld(), InteractWidgetClass);
	InteractWidget->FurnaceCraftingInventory->SetOwner(Inventory);
	InteractWidget->FurnaceFuelInventory->SetOwner(Inventory);
}


void AFurnace::RunningTick(float DeltaTime)
{
	Super::RunningTick(DeltaTime);
	UE_LOG(LogTemp, Log, TEXT(":ASDF"));
}

bool AFurnace::Interact_Implementation(ASpaceSmithCharacterController* Controller)
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

				InteractWidget->FurnaceCraftingInventory->Clear();
				InteractWidget->FurnaceFuelInventory->Clear();
				InteractWidget->PlayerInventory->Clear();

				const TArray<UInventorySlot*>& PlayerItems = Controller->Inventory->GetItems();
				for (auto & Item : PlayerItems)
				{
					InteractWidget->PlayerInventory->Add(Item);
				}

				const TArray<UInventorySlot*>& MachineItems = Inventory->GetItems();
				int32 HalfNum = MachineItems.Num() / 2;

				for (int32 Index = 0; Index < HalfNum; Index++)
				{
					InteractWidget->FurnaceFuelInventory->Add(MachineItems[Index]);
				}

				for (int32 Index = HalfNum; Index < MachineItems.Num(); Index++)
				{
					InteractWidget->FurnaceCraftingInventory->Add(MachineItems[Index]);
				}

				bInteractWidgetOpen = true;
			}
			return true;
		}
	}

	return false;
}