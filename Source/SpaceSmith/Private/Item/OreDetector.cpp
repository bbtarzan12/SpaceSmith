// Fill out your copyright notice in the Description page of Project Settings.


#include "OreDetector.h"
#include <Materials/MaterialParameterCollection.h>
#include <Materials/MaterialParameterCollectionInstance.h>

AOreDetector::AOreDetector() : Super()
{
	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> ScanParameterFinder(TEXT("MaterialParameterCollection'/Game/SpaceSmith/Materials/PostProcess/MPC_ScanFX'"));
	if (ScanParameterFinder.Succeeded())
	{
		ScanParameterCollection = ScanParameterFinder.Object;
	}
}

void AOreDetector::BeginPlay()
{
	Super::BeginPlay();
	ScanParameter = GetWorld()->GetParameterCollectionInstance(ScanParameterCollection);
}

void AOreDetector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndFire();
	Super::EndPlay(EndPlayReason);
}

void AOreDetector::Fire()
{
	if (!CanFire())
		return;

	if (bFire)
		return;

	StartFire();
}

void AOreDetector::StartFire()
{
	ScanParameter->SetVectorParameterValue(TEXT("CenterLocation"), GetActorLocation());
	bFire = true;
	ScanAmount = 0;
}

void AOreDetector::Tick(float DeltaTime)
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

void AOreDetector::EndFire()
{
	LastUsedTime = GetWorld()->GetTimeSeconds();
	bFire = false;
	if(ScanParameter)
		ScanParameter->SetScalarParameterValue(TEXT("Blend"), 0);
}