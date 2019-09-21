// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTable/MachineDataTable.h"
#include "Interface/Select.h"
#include "BaseMachine.generated.h"

UCLASS()
class SPACESMITH_API ABaseMachine : public AActor, public ISelect
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMachine();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(FMachineRow Machine);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Select();
	virtual bool Select_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Deselect();
	virtual bool Deselect_Implementation() override;

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
