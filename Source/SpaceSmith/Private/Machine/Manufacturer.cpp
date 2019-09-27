// Fill out your copyright notice in the Description page of Project Settings.


#include "Manufacturer.h"
#include "InventoryWidget.h"
#include "ManufacturerInteractWidget.h"
#include "SpaceSmithCharacterController.h"
#include "InventoryComponent.h"
#include "SpaceSmithGameMode.h"
#include "ManufacturerRuleEntry.h"

AManufacturer::AManufacturer()
{
	static ConstructorHelpers::FClassFinder<UManufacturerInteractWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/SpaceSmith/UMG/Machine/WBP_ManufacturerInteract'"));
	if (WidgetAsset.Succeeded())
	{
		InteractWidgetClass = WidgetAsset.Class;
	}

	bMachineTickable = true;
	bRunning = true;
}

void AManufacturer::BeginPlay()
{
	Super::BeginPlay();

	if (UInventoryComponent* Inventory = GetInventory(TEXT("Crafting")))
	{
		bInteractWidgetOpen = false;
		InteractWidget = CreateWidget<UManufacturerInteractWidget>(GetWorld(), InteractWidgetClass);
		InteractWidget->ManufacturerInventory->SetOwner(Inventory);
		InteractWidget->Manufacturer = this;

		Inventory->OnAnyChange.AddDynamic(this, &AManufacturer::ReloadInventory);
	}	
}

void AManufacturer::RunningTick(float DeltaTime)
{
	Super::RunningTick(DeltaTime);

	if (bCrafting)
	{
		Energy += 1;

		if (Energy < MaxEnergy)
		{
			return;
		}

		if (UInventoryComponent* Inventory = GetInventory(TEXT("Crafting")))
		{
			for (auto & Out : SelectedRule.Info.Out)
			{
				if (FItemRow* Row = Out.Handle.GetRow<FItemRow>(TEXT("")))
				{
					for (int32 Num = 0; Num < Out.Num; Num++)
					{
						Inventory->AddItem(*Row);
					}
				}
			}
		}

		Energy = 0;

		if (InteractWidget->NumCraft > 0)
		{
			InteractWidget->DownAmount();

			if (UInventoryComponent* Inventory = GetInventory(TEXT("Crafting")))
			{
				for (auto & In : SelectedRule.Info.In)
				{
					if (FItemRow* Row = In.Handle.GetRow<FItemRow>(TEXT("")))
					{
						Inventory->RemoveItem(*Row, In.Num);
					}
				}
			}
		}
		else
		{
			bCrafting = false;
		}	

		ReloadInventory();
	}
}

bool AManufacturer::Interact_Implementation(ASpaceSmithCharacterController* Controller)
{
	if (Super::Interact_Implementation(Controller))
	{
		if (Controller && Controller->Inventory)
		{
			if (UInventoryComponent* Inventory = GetInventory(TEXT("Crafting")))
			{
				if (bInteractWidgetOpen)
				{
					InteractWidget->RemoveFromViewport();
					Controller->ShowViewportWidget(false);
					bInteractWidgetOpen = false;
					CurrentController = nullptr;
				}
				else
				{
					CurrentController = Controller;
					InteractWidget->AddToViewport(5);
					InteractWidget->PlayerInventory->SetOwner(Controller->Inventory);
					Controller->ShowViewportWidget(true);

					InteractWidget->ManufacturerInventory->Clear();
					InteractWidget->PlayerInventory->Clear();

					const TArray<UInventorySlot*>& PlayerItems = Controller->Inventory->GetItems();
					for (auto & Item : PlayerItems)
					{
						InteractWidget->PlayerInventory->Add(Item);
					}

					const TArray<UInventorySlot*>& CraftingItems = Inventory->GetItems();
					for (auto & Item : CraftingItems)
					{
						InteractWidget->ManufacturerInventory->Add(Item);
					}

					bInteractWidgetOpen = true;
					ReloadInventory();
				}
				return true;
			}
		}
	}

	return false;
}

void AManufacturer::SelectRule(FCraftingRuleRow Rule)
{
	SelectedRule = Rule;
	ReloadInventory();
}

void AManufacturer::StartCrafting()
{
	if (bCrafting)
		return;

	if (SelectedRule.Info.In.Num() == 0)
	{
		return;
	}

	if (!bCanCraft)
	{
		return;
	}

	if (InteractWidget->NumCraft <= 0)
	{
		return;
	}

	InteractWidget->DownAmount();
	if (UInventoryComponent* Inventory = GetInventory(TEXT("Crafting")))
	{
		for (auto & In : SelectedRule.Info.In)
		{
			if (FItemRow* Row = In.Handle.GetRow<FItemRow>(TEXT("")))
			{
				Inventory->RemoveItem(*Row, In.Num);
			}
		}
	}

	bCrafting = true;
	MaxEnergy = SelectedRule.RequireEnergy;

	ReloadInventory();	
}

void AManufacturer::ReloadInventory()
{
	if (!CurrentController)
	{
		return;
	}

	bCanCraft = true;
	if (UInventoryComponent* Inventory = GetInventory(TEXT("Crafting")))
	{
		int32 MaxCraft = 99999;
		InteractWidget->InTileView->ClearListItems();
		for (auto & In : SelectedRule.Info.In)
		{
			if (FItemRow* Row = In.Handle.GetRow<FItemRow>(TEXT("")))
			{
				UManufacturerRuleInformation* RuleInformation = NewObject<UManufacturerRuleInformation>();
				RuleInformation->Row = *Row;
				RuleInformation->RequireAmount = In.Num;
				RuleInformation->bCanCraft = Inventory->Contains(RuleInformation->Row, RuleInformation->RequireAmount);
				RuleInformation->HasAmount = Inventory->Count(*Row);
				MaxCraft = FMath::Min(MaxCraft, RuleInformation->HasAmount / RuleInformation->RequireAmount);

				if (!RuleInformation->bCanCraft)
					bCanCraft = false;

				InteractWidget->InTileView->AddItem(RuleInformation);
			}
		}

		if (MaxCraft == 99999)
		{
			MaxCraft = 0;
		}

		InteractWidget->MaxCraft = MaxCraft;

		TArray<FItemRow> OutItems;
		InteractWidget->OutTileView->ClearListItems();
		for (auto & Out : SelectedRule.Info.Out)
		{
			if (FItemRow* Row = Out.Handle.GetRow<FItemRow>(TEXT("")))
			{
				UManufacturerRuleInformation* RuleInformation = NewObject<UManufacturerRuleInformation>();
				RuleInformation->Row = *Row;
				RuleInformation->RequireAmount = Out.Num;
				RuleInformation->bCanCraft = true;
				RuleInformation->HasAmount = CurrentController->Inventory->Count(*Row) + Inventory->Count(*Row);
				InteractWidget->OutTileView->AddItem(RuleInformation);
				OutItems.Emplace(*Row);
			}
		}

		if (!Inventory->CanAddItems(OutItems))
			bCanCraft = false;

		if (ASpaceSmithGameMode* GameMode = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (CurrentController)
			{
				InteractWidget->CraftingRuleListView->ClearListItems();
				for (auto & Rule : GameMode->ManufacturerCraftingRules)
				{
					TArray<UManufacturerRuleInformation*> Outs;
					for (auto & Out : Rule.Info.Out)
					{
						if (FItemRow* Row = Out.Handle.GetRow<FItemRow>(TEXT("")))
						{
							UManufacturerRuleInformation* RuleInformation = NewObject<UManufacturerRuleInformation>();
							RuleInformation->Row = *Row;
							RuleInformation->RequireAmount = Out.Num;
							RuleInformation->HasAmount = CurrentController->Inventory->Count(*Row) + Inventory->Count(*Row);
							Outs.Emplace(RuleInformation);
						}
					}
					UManufacturerRuleEntryObject* EntryObject = NewObject<UManufacturerRuleEntryObject>();
					EntryObject->Outs = Outs;
					EntryObject->Manufacturer = this;
					EntryObject->Rule = Rule;
					InteractWidget->CraftingRuleListView->AddItem(EntryObject);
				}
			}
		}

		if (SelectedRule.Info.In.Num() == 0)
		{
			bCanCraft = false;
		}
	}
	else
	{
		bCanCraft = false;
	}
}
