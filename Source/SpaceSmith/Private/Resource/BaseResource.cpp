// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseResource.h"
#include "BaseItem.h"
#include "ItemDataTable.h"

// Sets default values
ABaseResource::ABaseResource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ABaseResource::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseResource::Initialize(FResourceRow Resource)
{
	Data = Resource;
	Mesh->SetStaticMesh(Resource.Mesh);
}

// Called every frame
void ABaseResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseResource::Absorb(ASpaceSmithCharacterController* Controller)
{
	if (FItemRow* ItemRow = Data.AbsorbInformation.Row.GetRow<FItemRow>(TEXT("Can not find ItemRow")))
	{
		for (int32 Num = 0; Num < Data.AbsorbInformation.Num; Num++)
		{
			if (ABaseItem* AddingItem = Cast<ABaseItem>(GetWorld()->SpawnActor<ABaseItem>(ItemRow->Class, GetActorTransform())))
			{
				AddingItem->Initialize(*ItemRow);
				if (!Controller->AddItemToInventory(AddingItem, true))
				{
					AddingItem->SetActorLocation(GetActorLocation() + FVector(0, 0, Num * 25.0f));
				}
			}
		}
	}
}

