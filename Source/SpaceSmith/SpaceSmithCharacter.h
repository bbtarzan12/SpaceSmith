// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Public/BaseMachine.h"
#include "SpaceSmithCharacter.generated.h"

class UInputComponent;
class ABaseItem;
class UInventorySlot;
class IPick;
class ISelect;
class UCameraComponent;

UCLASS(config=Game)
class ASpaceSmithCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpaceSmithCharacter();
	virtual void Tick(float DeltaTime) override;
	void PickUp();
	void Drop();
	void Slot(UInventorySlot* Slot);

	UFUNCTION(BlueprintCallable, Category = "Pickable")
	FORCEINLINE bool HasHoldingPickable() const { return HoldingPickable.GetObject(); }

	UFUNCTION(BlueprintCallable, Category = "Select")
	FORCEINLINE bool HasSelectable() const { return Selectable.GetObject() != nullptr; }

	FORCEINLINE UCameraComponent* GetCameraController() const { return FirstPersonCameraComponent; }

	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

protected:
	virtual void BeginPlay() override;

private:
	void ResetHolding();

	void TraceObject();
	void HoldItem(float DeltaTime);

	void OnFire(); //Left Click
	void OnFireEnd();
	void OnHold(); //G
	void OnAction(); //F
	void OnInteract(); //E
	void OnInventory(); //I

	void SlotKey1();
	void SlotKey2();
	void SlotKey3();
	void SlotKey4();
	void SlotKey5();
	void SlotKey6();
	void SlotKey7();
	void SlotKey8();
	void SlotKey9();
	void SlotKey0();

private:
	/** First person camera */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Pick")
	class UPhysicsConstraintComponent* HoldingPhysicsJoint;

	UPROPERTY(VisibleAnywhere, Category = "Pick")
	class UStaticMeshComponent* HoldingSlot;

	UPROPERTY(VisibleAnywhere, Category = "Pick")
	class ASpaceSmithCharacterController* CharacterController;

	UPROPERTY(EditAnywhere, Category = "Select")
	float HoldingDistance;

	UPROPERTY(EditAnywhere, Category = "Select")
	float SelectDistance;

	UPROPERTY(VisibleAnywhere, Category = "Select")
	TScriptInterface<ISelect> Selectable;

	UPROPERTY(VisibleAnywhere, Category = "Pick")
	TScriptInterface<IPick> HoldingPickable;

	UPROPERTY(VisibleAnywhere, Category = "Pick")
	FHitResult CurrentSelectableHitResult;

	UPROPERTY(VisibleAnywhere, Category = "Slot")
	class ABaseItem* SlotItem;

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

};

