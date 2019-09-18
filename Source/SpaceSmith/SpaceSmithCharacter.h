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

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

};

