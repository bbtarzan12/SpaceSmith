// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceDetector.h"
#include <Materials/MaterialParameterCollection.h>
#include <Materials/MaterialParameterCollectionInstance.h>

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