// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/DataTable.h>
#include "DataTable/ItemDataTable.h"
#include "Interface/Interact.h"
#include "Interface/Select.h"
#include "Interface/Pick.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	Idle,
	Hold
};

UCLASS()
class SPACESMITH_API ABaseItem : public AActor, public IInteract, public ISelect, public IPick
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool PickUp(AActor* Actor);
	virtual bool PickUp_Implementation(AActor* Actor) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Drop();
	virtual bool Drop_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Interact(AController* Controller);
	virtual bool Interact_Implementation(AController* Controller) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Select(FHitResult HitResult);
	virtual bool Select_Implementation(FHitResult HitResult) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Deselect();
	virtual bool Deselect_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetPickInformationText();
	virtual FText GetPickInformationText_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetInteractInformationText();
	virtual FText GetInteractInformationText_Implementation() override;

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

	UPROPERTY(VisibleAnywhere)
	EItemState State;

};
