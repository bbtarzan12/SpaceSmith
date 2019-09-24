// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceAbsorber.h"
#include <Materials/MaterialParameterCollection.h>
#include <Materials/MaterialParameterCollectionInstance.h>
#include <Camera/CameraComponent.h>
#include "SpaceSmithCharacterController.h"

AResourceAbsorber::AResourceAbsorber() : Super()
{
	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> WormHoleParameterFinder(TEXT("MaterialParameterCollection'/Game/SpaceSmith/Materials/PostProcess/MPC_WormHole'"));
	if (WormHoleParameterFinder.Succeeded())
	{
		WormHoleParameterCollection = WormHoleParameterFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> WormHoleMaterialFinder(TEXT("Material'/Game/SpaceSmith/Materials/PostProcess/M_WormHole'"));
	if (WormHoleMaterialFinder.Succeeded())
	{
		WormHoleMaterial = WormHoleMaterialFinder.Object;
	}

	AbsorbeLocation = CreateDefaultSubobject<USceneComponent>(TEXT("AbsorbeLocation"));
	AbsorbeLocation->AddLocalOffset(FVector(50.0f, 0.0f, 0.0f));
	AbsorbeLocation->SetupAttachment(RootComponent);
}

void AResourceAbsorber::BeginPlay()
{
	Super::BeginPlay();
	WormHoleParameter = GetWorld()->GetParameterCollectionInstance(WormHoleParameterCollection);
	WormHoleMaterialDynamic = UMaterialInstanceDynamic::Create(WormHoleMaterial, this);
}

void AResourceAbsorber::Fire()
{
	if (!OwnerController)
		return;

	bFire = true;
	FireTime = 0;
	TargetActor = nullptr;
}

void AResourceAbsorber::FireEnd()
{
	if (!OwnerController)
		return;

	bFire = false;
	LastUsedTime = GetWorld()->GetTimeSeconds();
}

void AResourceAbsorber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwnerController)
		return;

	if (FireTime >= 3.0f)
	{
		TargetActor->Destroy();
		TargetActor = nullptr;
		FireTime = 0;
		ResetParameter();
		return;
	}

	if (FireTime >= 2.0f)
	{
		if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(TargetActor->GetRootComponent()))
		{
			for (int32 Index = 0; Index < MeshComponent->GetNumMaterials(); Index++)
			{
				MeshComponent->SetMaterial(Index, WormHoleMaterialDynamic);
			}
		}
		Falloff += DeltaTime * 10.0f;
		Radius += DeltaTime * 5000.0f;
		WormHoleParameter->SetScalarParameterValue(TEXT("VertCrushRadius"), Radius);
		WormHoleParameter->SetScalarParameterValue(TEXT("VertCrushFalloff"), Falloff);
		WormHoleParameter->SetVectorParameterValue(TEXT("CenterLocation"), AbsorbeLocation->GetComponentLocation());
		FireTime += DeltaTime;
		return;
	}

	if (bFire)
	{
		UCameraComponent* CameraComponent = OwnerController->GetPawnCameraComponent();
		FVector Start = CameraComponent->GetComponentLocation();
		FVector ForwardVector = CameraComponent->GetForwardVector();
		FVector End = (Start + ForwardVector * 400.0f);
		FCollisionQueryParams CQP;
		CQP.AddIgnoredActor(this);
		CQP.AddIgnoredActor(OwnerController->GetPawn());
		FHitResult HitResult;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Camera, CQP))
		{
			if (HitResult.GetActor()->GetName().Contains(TEXT("Test")))
			{
				if (TargetActor)
				{
					if (TargetActor == HitResult.GetActor())
					{
						GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("%f"), FireTime));
						FireTime += DeltaTime;
					}
					else
					{
						TargetActor = HitResult.GetActor();
						FireTime = 0;
						ResetParameter();
					}
				}
				else
				{
					TargetActor = HitResult.GetActor();
					FireTime = 0;
					ResetParameter();
				}
			}
			else
			{
				TargetActor = nullptr;
				FireTime = 0;
				ResetParameter();
			}
		}
		else
		{
			TargetActor = nullptr;
			FireTime = 0;
			ResetParameter();
		}
	}
}

void AResourceAbsorber::ResetParameter()
{
	Falloff = 0;
	Radius = 0;
	WormHoleParameter->SetScalarParameterValue(TEXT("VertCrushRadius"), Falloff);
	WormHoleParameter->SetScalarParameterValue(TEXT("VertCrushFalloff"), Radius);
}