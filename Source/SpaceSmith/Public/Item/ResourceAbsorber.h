// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ResourceAbsorber.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API AResourceAbsorber : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AResourceAbsorber();

	virtual void Fire() override;
	virtual void FireEnd() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

private:
	void ResetParameter();

private:
	bool bFire;
	float FireTime;
	float Radius;
	float Falloff;

	UPROPERTY()
	class ABaseResource* TargetResource;

	UPROPERTY()
	class USceneComponent* AbsorbeLocation;

	UPROPERTY()
	class UMaterialParameterCollectionInstance* WormHoleParameter;

	UPROPERTY()
	class UMaterialParameterCollection* WormHoleParameterCollection;

	UPROPERTY()
	class UMaterial* WormHoleMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* WormHoleMaterialDynamic;
	
};
