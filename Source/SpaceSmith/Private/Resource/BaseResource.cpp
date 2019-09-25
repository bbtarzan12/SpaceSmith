// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseResource.h"
#include "BaseItem.h"
#include "ItemDataTable.h"
#include "ResourceOnDetectPopUpWidget.h"
#include <WidgetComponent.h>

// Sets default values
ABaseResource::ABaseResource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	DetectorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Detector Widget Component"));
	DetectorWidgetComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UResourceOnDetectPopUpWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/SpaceSmith/UMG/Resource/WBP_ResourceOnDetectPopUp'"));
	if (WidgetAsset.Succeeded())
	{
		WidgetClass = WidgetAsset.Class;
	}

	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ABaseResource::BeginPlay()
{
	Super::BeginPlay();

	DetectorWidget = CreateWidget<UResourceOnDetectPopUpWidget>(GetWorld(), WidgetClass);
	DetectorWidget->Resource = this;
	DetectorWidgetComponent->SetWidget(DetectorWidget);
	DetectorWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DetectorWidgetComponent->SetDrawAtDesiredSize(true);
	DetectorWidgetComponent->SetWorldLocation(GetActorLocation());
}

// Called every frame
void ABaseResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDetect)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();

		if (CurrentTime - DetectTime >= 5.0f)
		{
			bDetect = false;
			DetectorWidget->OnEndDetect();
		}
	}
}

void ABaseResource::OnDetect()
{
	DetectTime = GetWorld()->GetTimeSeconds();
	if (!bDetect)
	{
		bDetect = true;
		DetectorWidget->OnInitDetect();
	}
}

void ABaseResource::Initialize(FResourceRow Resource)
{
	Data = Resource;
	Mesh->SetStaticMesh(Resource.Mesh);
}

void ABaseResource::SetCollisionProfile(FName ProfileName)
{
	Mesh->SetCollisionProfileName(ProfileName);
}

void ABaseResource::AbsorbStart()
{
	SetActorEnableCollision(false);
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

	Destroy();
}

