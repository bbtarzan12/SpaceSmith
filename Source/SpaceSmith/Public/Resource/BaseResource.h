// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceDataTable.h"
#include "SpaceSmithCharacterController.h"
#include "BaseResource.generated.h"

UCLASS()
class SPACESMITH_API ABaseResource : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseResource();

	void Initialize(FResourceRow Resource);
	void SetCollisionProfile(FName ProfileName);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnDetect();
	virtual void AbsorbStart();
	virtual void Absorb(ASpaceSmithCharacterController* Controller);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FResourceRow Data;


protected:
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* DetectorWidgetComponent;

	UPROPERTY(VisibleAnywhere)
	class UResourceOnDetectPopUpWidget* DetectorWidget;

	TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY(VisibleAnywhere)
	bool bDetect;

	UPROPERTY(VisibleAnywhere)
	float DetectTime;



};
