// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SpaceSmithCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Public/BaseItem.h"
#include <PhysicsEngine/PhysicsConstraintComponent.h>
#include "Public/SpaceSmithCharacterController.h"



//////////////////////////////////////////////////////////////////////////
// ASpaceSmithCharacter

ASpaceSmithCharacter::ASpaceSmithCharacter()
{
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	HoldingSlot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HoldingSlot"));
	HoldingSlot->SetupAttachment(FirstPersonCameraComponent);

	HoldingPhysicsJoint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("HoldingPhysicsJoint"));
	HoldingPhysicsJoint->SetupAttachment(HoldingSlot);

	GetMesh()->SetupAttachment(FirstPersonCameraComponent);
}

void ASpaceSmithCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	CharacterController = Cast<ASpaceSmithCharacterController>(GetController());

	ensure(CharacterController != nullptr);
}

void ASpaceSmithCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasHoldingItem())
	{
		HoldItem(DeltaTime);
	}
	else
	{
		TraceItem();
	}
}

void ASpaceSmithCharacter::PickUp()
{
	if (HasSelectedItem())
	{
		HoldingItem = SelectedItem;
		HoldingItem->Select();
		SelectedItem = nullptr;
		HoldingItem->PickUp(Cast<AActor>(this));

		HoldingSlot->SetWorldLocation(CurrentItemHitResult.ImpactPoint);
		HoldingPhysicsJoint->ConstraintActor1 = this;
		HoldingPhysicsJoint->ConstraintActor2 = HoldingItem;
		HoldingPhysicsJoint->SetConstrainedComponents(Cast<UPrimitiveComponent>(HoldingSlot), NAME_None, Cast<UPrimitiveComponent>(HoldingItem->GetRootComponent()), NAME_None);
	}
}

void ASpaceSmithCharacter::Drop()
{
	if (HasHoldingItem())
	{
		HoldingItem->Drop();
		HoldingItem->Deselect();

		HoldingPhysicsJoint->BreakConstraint();
		HoldingPhysicsJoint->ConstraintActor1 = nullptr;
		HoldingPhysicsJoint->ConstraintActor2 = nullptr;

		HoldingItem = nullptr;
	}
}

void ASpaceSmithCharacter::TraceItem()
{
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector End = (Start + ForwardVector * SelectDistance);
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(CurrentItemHitResult, Start, End, ECollisionChannel::ECC_Camera, CQP) && CurrentItemHitResult.Actor->IsA(ABaseItem::StaticClass()))
	{
		if (CurrentItemHitResult.Actor != SelectedItem)
		{
			if (SelectedItem)
			{
				SelectedItem->Deselect();
			}

			SelectedItem = Cast<ABaseItem>(CurrentItemHitResult.GetActor());
			SelectedItem->Select();
		}
	}
	else
	{
		if (SelectedItem)
		{
			SelectedItem->Deselect();
			SelectedItem = nullptr;
		}
	}
}

void ASpaceSmithCharacter::HoldItem(float DeltaTime)
{
	FVector Location = FirstPersonCameraComponent->GetComponentLocation() + FirstPersonCameraComponent->GetForwardVector() * HoldingDistance;
	HoldingSlot->SetWorldLocation(FMath::Lerp(HoldingSlot->GetComponentLocation(), Location, DeltaTime * 10.0f));
}

void ASpaceSmithCharacter::OnHold()
{
	if (HasHoldingItem())
	{
		Drop();
	}
	else if (HasSelectedItem())
	{
		PickUp();
	}
}

void ASpaceSmithCharacter::OnAction()
{
	
}

void ASpaceSmithCharacter::OnInteract()
{
	if (HasSelectedItem())
	{
		CharacterController->AddItemToInventory(SelectedItem->ItemData);
	}
}

void ASpaceSmithCharacter::OnInventory()
{
	CharacterController->ToggleInventoryUMG();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASpaceSmithCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpaceSmithCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceSmithCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASpaceSmithCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASpaceSmithCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASpaceSmithCharacter::OnInteract);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &ASpaceSmithCharacter::OnAction);
	PlayerInputComponent->BindAction("Hold", IE_Pressed, this, &ASpaceSmithCharacter::OnHold);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ASpaceSmithCharacter::OnInventory);

}

void ASpaceSmithCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASpaceSmithCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASpaceSmithCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASpaceSmithCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
