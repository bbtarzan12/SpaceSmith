// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Public/SpaceSmithCharacterController.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCustomDepthStencilValue(252);
	Mesh->SetCollisionProfileName(TEXT("Item"));

	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	FItemRow* Row = DataTableHandle.GetRow<FItemRow>(TEXT("Can not found FItemRow"));
	if (Row)
	{
		Initialize(*Row);
	}
}

void ABaseItem::Initialize(FItemRow Item)
{
	Data = Item;
	Mesh->SetStaticMesh(Item.Mesh);
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABaseItem::PickUp_Implementation(AActor* Actor)
{
	HoldingActor = Actor;
	Mesh->SetSimulatePhysics(true);
	bPicked = true;
	return true;
}


bool ABaseItem::Drop_Implementation()
{
	HoldingActor = nullptr;
	bPicked = false;
	return true;
}

bool ABaseItem::Select_Implementation(FHitResult HitResult)
{
	Mesh->SetRenderCustomDepth(true);
	bSelected = true;
	return true;
}

bool ABaseItem::Deselect_Implementation()
{
	Mesh->SetRenderCustomDepth(false);
	bSelected = false;
	return true;
}

bool ABaseItem::Interact_Implementation(AController* Controller)
{
	// Item�� Interact�� ��� ���� �� �� ����� ��
	if (Controller->IsA(ASpaceSmithCharacterController::StaticClass()))
	{
		Cast<ASpaceSmithCharacterController>(Controller)->AddItemToInventory(this);
	}
	return true;
}
