// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "OreDetector.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API AOreDetector : public ABaseItem
{
	GENERATED_BODY()

public:
	AOreDetector();

	virtual void Fire() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

private:
	void StartFire();
	void EndFire();

private:
	bool bFire;
	float ScanAmount;


	UPROPERTY()
	class UMaterialParameterCollectionInstance* ScanParameter;
	
	UPROPERTY()
	class UMaterialParameterCollection* ScanParameterCollection;
	
};
