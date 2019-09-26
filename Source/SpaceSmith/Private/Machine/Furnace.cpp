// Fill out your copyright notice in the Description page of Project Settings.


#include "Furnace.h"
#include "InventoryWidget.h"
#include "SpaceSmithCharacterController.h"
#include "InventoryComponent.h"
#include "FurnaceInteractWidget.h"
#include "SpaceSmithGameMode.h"
#include "ItemDataTable.h"

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

	CurrentFuel = 0;
	bInteractWidgetOpen = false;
	InteractWidget = CreateWidget<UFurnaceInteractWidget>(GetWorld(), InteractWidgetClass);
	InteractWidget->FurnaceCraftingInventory->SetOwner(GetInventory(TEXT("Crafting")));
	InteractWidget->FurnaceFuelInventory->SetOwner(GetInventory(TEXT("Fuel")));
	InteractWidget->Furnace = this;
}


void AFurnace::RunningTick(float DeltaTime)
{
	Super::RunningTick(DeltaTime);

	Fuel();
	Crafting();
}

void AFurnace::Fuel()
{
	CurrentFuel = FMath::Clamp(CurrentFuel - 1.0f, 0.0f, MaxFuel);
	Energy = FMath::Clamp(Energy + 1.0f, 0.0f, MaxEnergy);

	if (UInventoryComponent* Inventory = GetInventory(TEXT("Fuel")))
	{
		const TArray<UInventorySlot*>& FuelItems = Inventory->GetItems();
		for (auto& Item : FuelItems)
		{
			if (Item->Row.bFuel)
			{
				if (Item->Row.FuelEfficiency + CurrentFuel > MaxFuel)
					continue;

				Inventory->RemoveItem(Item, 1);
				CurrentFuel += Item->Row.FuelEfficiency;
				return;
			}
		}
	}

	if (CurrentFuel == 0)
	{
		bRunning = false;
		return;
	}
}

void AFurnace::Crafting()
{
	if (UInventoryComponent* Inventory = GetInventory(TEXT("Crafting")))
	{
		if (ASpaceSmithGameMode* GameMode = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode()))
		{
			for (auto & Rule : GameMode->FurnaceCraftingRules)
			{
				if (Rule.RequireEnergy > Energy)
					continue;

				TArray<FItemRow> OutItems;
				TMap<FItemRow, int32> InInformation;
				TMap<FItemRow, int32> OutInformation;
				for (auto & Pair : Rule.Rule.Out)
				{
					if (FItemRow* Row = Pair.Handle.GetRow<FItemRow>(TEXT("")))
					{
						OutItems.Emplace(*Row);
						OutInformation.Emplace(*Row, Pair.Num);
					}
				}
				if (!Inventory->CanAddItems(OutItems))
					continue;

				bool HasAllItems = true;
				for (auto & Pair : Rule.Rule.In)
				{
					if (FItemRow* Row = Pair.Handle.GetRow<FItemRow>(TEXT("")))
					{
						if (Inventory->Contains(*Row, Pair.Num))
						{
							InInformation.Emplace(*Row, Pair.Num);
						}
						else
						{
							HasAllItems = false;
							break;
						}
					}
				}

				if (HasAllItems)
				{
					Energy -= Rule.RequireEnergy;

					for (TPair<FItemRow, int32>& In : InInformation)
					{
						Inventory->RemoveItem(In.Key, In.Value);
					}

					for (TPair<FItemRow, int32>& Out : OutInformation)
					{
						for (int32 Num = 0; Num < Out.Value; Num++)
						{
							Inventory->AddItem(Out.Key);
						}
					}
					return;
				}
			}
		}
	}
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

				const TArray<UInventorySlot*>& CraftingItems = GetInventory(TEXT("Crafting"))->GetItems();
				for (auto & Item : CraftingItems)
				{
					InteractWidget->FurnaceCraftingInventory->Add(Item);
				}

				const TArray<UInventorySlot*>& FuelItems = GetInventory(TEXT("Fuel"))->GetItems();
				for (auto & Item : FuelItems)
				{
					InteractWidget->FurnaceFuelInventory->Add(Item);
				}

				bInteractWidgetOpen = true;
			}
			return true;
		}
	}

	return false;
}