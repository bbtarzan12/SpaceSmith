// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTable/MachineDataTable.h"
#include "BaseMachine.generated.h"

UCLASS()
class SPACESMITH_API ABaseMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMachine();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(FMachineRow Machine);
	void Select(FHitResult HitResult);
	void Deselect();

	FORCEINLINE const FText& GetName() const { return Data.Name; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FMachineRow Data;

private:
	UPROPERTY(EditAnywhere)
	FDataTableRowHandle DataTableHandle;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* InformationWidget;

	UPROPERTY(VisibleAnywhere)
	class UInventoryComponent* Inventory;


};
