// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMachine.h"
#include "Public/Component/InventoryComponent.h"
#include "Public/Widget/MachineInformationWidget.h"
#include <WidgetComponent.h>

// Sets default values
ABaseMachine::ABaseMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	
	InformationWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Information Widget Component"));
	InformationWidget->SetupAttachment(RootComponent);

	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ABaseMachine::BeginPlay()
{
	Super::BeginPlay();

	InformationWidget->SetHiddenInGame(true);
	InformationWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InformationWidget->SetDrawAtDesiredSize(true);
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

	if (Data.bInventory)
	{
		Inventory = NewObject<UInventoryComponent>(this, TEXT("InventoryComponent"));
		Inventory->RegisterComponent();
		Inventory->SetCapacity(Data.Capacity);
		AddOwnedComponent(Inventory);
	}
}

// Called every frame
void ABaseMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseMachine::Select(FHitResult HitResult)
{
	FVector InformationWidgetLocation = InformationWidget->GetComponentLocation();
	FVector TargetLocaiton = HitResult.ImpactPoint;
	FVector InformationWidgetSmoothingLocation = FMath::Lerp(InformationWidgetLocation, TargetLocaiton, GetWorld()->DeltaTimeSeconds * 10.0f);
	InformationWidget->SetWorldLocation(InformationWidgetSmoothingLocation);
	InformationWidget->SetHiddenInGame(false);
}

void ABaseMachine::Deselect()
{
	InformationWidget->SetHiddenInGame(true);
}