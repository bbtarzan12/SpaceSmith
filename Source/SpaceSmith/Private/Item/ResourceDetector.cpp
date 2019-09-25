// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceDetector.h"
#include <Materials/MaterialParameterCollection.h>
#include <Materials/MaterialParameterCollectionInstance.h>
#include <Kismet/KismetSystemLibrary.h>
#include <EngineUtils.h>
#include <InstancedFoliageActor.h>
#include "SpaceSmithGameMode.h"
#include "BaseResource.h"
#include "ResourceDataTable.h"

AResourceDetector::AResourceDetector() : Super()
{
	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> ScanParameterFinder(TEXT("MaterialParameterCollection'/Game/SpaceSmith/Materials/PostProcess/MPC_ScanFX'"));
	if (ScanParameterFinder.Succeeded())
	{
		ScanParameterCollection = ScanParameterFinder.Object;
	}
}

void AResourceDetector::BeginPlay()
{
	Super::BeginPlay();
	ScanParameter = GetWorld()->GetParameterCollectionInstance(ScanParameterCollection);
}

void AResourceDetector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndFire();
	Super::EndPlay(EndPlayReason);
}

void AResourceDetector::Fire()
{
	if (!CanFire())
		return;

	if (bFire)
		return;

	StartFire();
}

void AResourceDetector::StartFire()
{
	ScanParameter->SetVectorParameterValue(TEXT("CenterLocation"), GetActorLocation());
	bFire = true;
	ScanAmount = 0;

	TArray<UActorComponent*> InstancedStaticMeshComponents;
	for (const auto& Foliage : TActorRange<AInstancedFoliageActor>(GetWorld()))
	{
		InstancedStaticMeshComponents = Foliage->GetComponentsByClass(UInstancedStaticMeshComponent::StaticClass());
	}

	for (auto & InstancedStaticMeshComponent : InstancedStaticMeshComponents)
	{
		if (UInstancedStaticMeshComponent* InstancedComponent = Cast<UInstancedStaticMeshComponent>(InstancedStaticMeshComponent))
		{
			TArray<int32> Indices = InstancedComponent->GetInstancesOverlappingSphere(GetActorLocation(), 5000, true);
			for (auto& Index : Indices)
			{
				FTransform FoliageTransform;
				InstancedComponent->GetInstanceTransform(Index, FoliageTransform, true);
				InstancedComponent->RemoveInstance(Index);

				const TMap<FString, FResourceRow>& ResourceMap = Cast<ASpaceSmithGameMode>(GetWorld()->GetAuthGameMode())->ResourceMap;

				if (ResourceMap.Contains(InstancedComponent->GetStaticMesh()->GetName()))
				{
					FResourceRow ResourceRow = ResourceMap[InstancedComponent->GetStaticMesh()->GetName()];
					if (ABaseResource* Resource = GetWorld()->SpawnActor<ABaseResource>(ResourceRow.Class, FoliageTransform))
					{
						Resource->Initialize(ResourceRow);
						Resource->SetCollisionProfile(InstancedComponent->GetCollisionProfileName());
						Resource->OnDetect();
					}
				}
			}
		}
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;
	if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), 5000, ObjectTypes, ABaseResource::StaticClass(), IgnoreActors, OutActors))
	{
		for (auto& OutActor : OutActors)
		{
			if (ABaseResource* Resource = Cast<ABaseResource>(OutActor))
			{
				Resource->OnDetect();
			}
		}
	}
}

void AResourceDetector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFire)
	{
		if (ScanAmount > 1.0f)
		{
			EndFire();
			return;
		}
		ScanAmount += DeltaTime * 0.5f;
		ScanParameter->SetScalarParameterValue(TEXT("Blend"), ScanAmount);
	}
}

void AResourceDetector::EndFire()
{
	LastUsedTime = GetWorld()->GetTimeSeconds();
	bFire = false;
	if(ScanParameter)
		ScanParameter->SetScalarParameterValue(TEXT("Blend"), 0);
}