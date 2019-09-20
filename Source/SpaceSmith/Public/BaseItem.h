// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/DataTable.h>
#include "DataTable/ItemDataTable.h"
#include "BaseItem.generated.h"

UCLASS()
class SPACESMITH_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PickUp(AActor* Actor);
	virtual void Drop();
	virtual void Select();
	virtual void Deselect();
	
	void Initialize(FItemRow Item);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FItemRow Data;

protected:
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle DataTableHandle;

	UPROPERTY(VisibleAnywhere)
	AActor* HoldingActor;

	UPROPERTY(VisibleAnywhere)
	bool bPicked;

	UPROPERTY(VisibleAnywhere)
	bool bSelected;



};
