// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpaceSmithCharacter.generated.h"

class UInputComponent;
class ABaseItem;

UCLASS(config=Game)
class ASpaceSmithCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpaceSmithCharacter();
	virtual void Tick(float DeltaTime) override;
	void PickUp();
	void Drop();

	UFUNCTION(BlueprintCallable, Category = "Pickable")
	FORCEINLINE bool HasHoldingItem() const { return HoldingItem != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Pickable")
	FORCEINLINE bool HasSelectedItem() const { return SelectedItem != nullptr; }

	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

protected:
	virtual void BeginPlay() override;

private:
	void TraceItem();
	void HoldItem(float DeltaTime);
	void OnHold(); //G
	void OnAction(); //F
	void OnInteract(); //E
	void OnInventory(); //I

private:
	/** First person camera */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, Category = Pick)
	class UPhysicsConstraintComponent* HoldingPhysicsJoint;

	UPROPERTY(VisibleAnywhere, Category = Pick)
	class UStaticMeshComponent* HoldingSlot;

	UPROPERTY(VisibleAnywhere, Category = Pick)
	class ASpaceSmithCharacterController* CharacterController;

	UPROPERTY(EditAnywhere, Category = Pick)
	float HoldingDistance;

	UPROPERTY(EditAnywhere, Category = Pick)
	float SelectDistance;

	UPROPERTY(VisibleAnywhere, Category = Pick)
	ABaseItem* SelectedItem;

	UPROPERTY(VisibleAnywhere, Category = Pick)
	ABaseItem* HoldingItem;

	UPROPERTY(VisibleAnywhere, Category = Pick)
	FHitResult CurrentItemHitResult;

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

};

