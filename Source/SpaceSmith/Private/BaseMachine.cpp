// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMachine.h"
#include "Public/Component/InventoryComponent.h"
#include "Public/Widget/MachineInformationWidget.h"
#include <WidgetComponent.h>
#include "SpaceSmithCharacterController.h"

// Sets default values
ABaseMachine::ABaseMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	InformationWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Information Widget Component"));
	InformationWidget->SetupAttachment(RootComponent);

	bMachineTickable = false;
	MachineTickInterval = 1.0f;

	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ABaseMachine::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(bMachineTickable);
	SetActorTickInterval(MachineTickInterval);
	InformationWidget->SetHiddenInGame(true);
	InformationWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InformationWidget->SetDrawAtDesiredSize(true);
	InformationWidget->SetWorldLocation(GetActorLocation());
	Cast<UMachineInformationWidget>(InformationWidget->GetUserWidgetObject())->Machine = this;

	FMachineRow* Row = DataTableHandle.GetRow<FMachineRow>(TEXT("Can not found FMachineRow"));
	if (Row)
	{
		Initialize(*Row);
	}
	
}

void ABaseMachine::Initialize(FMachineRow Machine)
{
	Data = Machine;
	Mesh->SetStaticMesh(Data.Mesh);

	for (auto& InventoryInformation : Data.Inventories)
	{
		UInventoryComponent* Inventory = NewObject<UInventoryComponent>(this, InventoryInformation.Name);
		Inventory->RegisterComponent();
		Inventory->SetCapacity(InventoryInformation.Capacity);
		Inventory->SetName(InventoryInformation.DisplayName);
		AddOwnedComponent(Inventory);

		Inventories.Emplace(InventoryInformation.Name, Inventory);
	}
}

// Called every frame
void ABaseMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bRunning)
		RunningTick(DeltaTime);
}

void ABaseMachine::RunningTick(float DeltaTime)
{

}

bool ABaseMachine::Select_Implementation(FHitResult HitResult)
{
	InformationWidget->SetWorldLocation(HitResult.ImpactPoint);
	InformationWidget->SetHiddenInGame(false);
	return true;
}

bool ABaseMachine::SelectTick_Implementation(FHitResult HitResult)
{
	FVector InformationWidgetLocation = InformationWidget->GetComponentLocation();
	FVector TargetLocaiton = HitResult.ImpactPoint;
	FVector InformationWidgetSmoothingLocation = FMath::VInterpTo(InformationWidgetLocation, TargetLocaiton, GetWorld()->GetDeltaSeconds(), 50.0f);
	InformationWidget->SetWorldLocation(InformationWidgetSmoothingLocation);
	InformationWidget->SetHiddenInGame(false);
	return true;
}

bool ABaseMachine::Deselect_Implementation()
{
	InformationWidget->SetHiddenInGame(true);
	return true;
}

bool ABaseMachine::Interact_Implementation(ASpaceSmithCharacterController* Controller)
{
	return true;
}

FText ABaseMachine::GetInteractInformationText_Implementation()
{
	return FText::FromStringTable("/Game/SpaceSmith/Data/StringTable/KeyInformation", FString(TEXT("Interact_Machine")));
}

bool ABaseMachine::Action_Implementation()
{
	if (bMachineTickable)
	{
		bRunning = !bRunning;
		return bRunning;
	}

	return false;
}

FText ABaseMachine::GetActionInformationText_Implementation()
{
	if (bMachineTickable)
	{
		if (bRunning)
		{
			return FText::FromStringTable("/Game/SpaceSmith/Data/StringTable/KeyInformation", FString(TEXT("Action_Machine_Running")));

		}
		else
		{
			return FText::FromStringTable("/Game/SpaceSmith/Data/StringTable/KeyInformation", FString(TEXT("Action_Machine_Off")));
		}
	}

	return FText::GetEmpty();
}