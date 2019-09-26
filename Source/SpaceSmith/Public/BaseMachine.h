// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataTable/MachineDataTable.h"
#include "Interface/Select.h"
#include "Interact.h"
#include "Action.h"
#include "InventoryComponent.h"
#include "BaseMachine.generated.h"

class ASpaceSmithCharacterController;

UCLASS()
class SPACESMITH_API ABaseMachine : public AActor, public ISelect, public IInteract, public IAction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMachine();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(FMachineRow Machine);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Select(FHitResult HitResult);
	virtual bool Select_Implementation(FHitResult HitResult) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool SelectTick(FHitResult HitResult);
	virtual bool SelectTick_Implementation(FHitResult HitResult) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Deselect();
	virtual bool Deselect_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Interact(ASpaceSmithCharacterController* Controller);
	virtual bool Interact_Implementation(ASpaceSmithCharacterController* Controller) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetInteractInformationText();
	virtual FText GetInteractInformationText_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Action();
	virtual bool Action_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetActionInformationText();
	virtual FText GetActionInformationText_Implementation() override;

	FORCEINLINE const FText& GetName() const { return Data.Name; };
	FORCEINLINE UInventoryComponent* GetInventory(FName Name) const { if (Inventories.Contains(Name)) { check(Inventories[Name]); return Inventories[Name]; } return nullptr; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void RunningTick(float DeltaTime);

public:
	FMachineRow Data;

protected:
	UPROPERTY(EditAnywhere)
	FDataTableRowHandle DataTableHandle;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* InformationWidget;

	UPROPERTY(VisibleAnywhere)
	bool bRunning;

	UPROPERTY(VisibleAnywhere)
	bool bMachineTickable;

	UPROPERTY(VisibleAnywhere)
	float MachineTickInterval;

private:
	UPROPERTY(VisibleAnywhere)
	TMap<FName, UInventoryComponent*> Inventories;


};
