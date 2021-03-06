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
#include "Public/Component/InventoryComponent.h"
#include "Public/Interface/Pick.h"
#include "Action.h"



//////////////////////////////////////////////////////////////////////////
// ASpaceSmithCharacter

ASpaceSmithCharacter::ASpaceSmithCharacter()
{
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

	if (CharacterController->GetViewportWidgetVisible())
		return;

	if (HasHoldingPickable())
	{
		HoldItem(DeltaTime);
	}
	else
	{
		TraceObject();
	}
}

void ASpaceSmithCharacter::PickUp()
{
	if (HasSelectable())
	{
		if (Selectable.GetObject()->GetClass()->ImplementsInterface(UPick::StaticClass()))
		{
			HoldingPickable.SetObject(Selectable.GetObject());
			HoldingPickable.SetInterface(Cast<IPick>(Selectable.GetObject()));

			IPick::Execute_PickUp(HoldingPickable.GetObject(), this);
			
			if (AActor* HoldingActor = Cast<AActor>(HoldingPickable.GetObject()))
			{
				HoldingSlot->SetWorldLocation(CurrentSelectableHitResult.ImpactPoint);
				HoldingPhysicsJoint->ConstraintActor1 = this;
				HoldingPhysicsJoint->ConstraintActor2 = HoldingActor;
				HoldingPhysicsJoint->SetConstrainedComponents(Cast<UPrimitiveComponent>(HoldingSlot), NAME_None, Cast<UPrimitiveComponent>(HoldingActor->GetRootComponent()), NAME_None);
			}
		}
	}
}

void ASpaceSmithCharacter::Drop()
{
	if (HasHoldingPickable())
	{
		IPick::Execute_Drop(HoldingPickable.GetObject());
		ResetHolding();
	}
}

void ASpaceSmithCharacter::ResetHolding()
{
	HoldingPhysicsJoint->BreakConstraint();
	HoldingPhysicsJoint->ConstraintActor1 = nullptr;
	HoldingPhysicsJoint->ConstraintActor2 = nullptr;

	HoldingPickable.SetObject(nullptr);
	HoldingPickable.SetInterface(nullptr);
}

void ASpaceSmithCharacter::TraceObject()
{
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector End = (Start + ForwardVector * SelectDistance);
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(CurrentSelectableHitResult, Start, End, ECollisionChannel::ECC_Camera, CQP))
	{
		if (CurrentSelectableHitResult.GetActor()->GetClass()->ImplementsInterface(USelect::StaticClass()))
		{
			if (Selectable.GetObject() == CurrentSelectableHitResult.GetActor())
			{
				ISelect::Execute_SelectTick(Selectable.GetObject(), CurrentSelectableHitResult);
			}
			else
			{
				if (Selectable.GetObject())
				{
					ISelect::Execute_Deselect(Selectable.GetObject());
					Selectable.SetObject(nullptr);
					Selectable.SetInterface(nullptr);
				}

				Selectable.SetObject(CurrentSelectableHitResult.GetActor());
				Selectable.SetInterface(Cast<ISelect>(CurrentSelectableHitResult.GetActor()));
				ISelect::Execute_Select(Selectable.GetObject(), CurrentSelectableHitResult);
			}
		}
		else
		{
			if (Selectable.GetObject())
			{
				ISelect::Execute_Deselect(Selectable.GetObject());
				Selectable.SetObject(nullptr);
				Selectable.SetInterface(nullptr);
			}
		}
		CharacterController->Select(CurrentSelectableHitResult.GetActor());
	}
	else
	{
		if (Selectable.GetObject())
		{
			ISelect::Execute_Deselect(Selectable.GetObject());
			Selectable.SetObject(nullptr);
			Selectable.SetInterface(nullptr);
		}
		CharacterController->Deselect();
	}
}

void ASpaceSmithCharacter::HoldItem(float DeltaTime)
{
	if (AActor* HoldingActor = Cast<AActor>(HoldingPickable.GetObject()))
	{
		if (HoldingActor->IsActorBeingDestroyed())
		{
			ResetHolding();
			return;
		}
	}

	FVector Location = FirstPersonCameraComponent->GetComponentLocation() + FirstPersonCameraComponent->GetForwardVector() * HoldingDistance;
	HoldingSlot->SetWorldLocation(FMath::Lerp(HoldingSlot->GetComponentLocation(), Location, DeltaTime * 10.0f));
	CharacterController->Select(Cast<AActor>(HoldingPickable.GetObject()));
}

void ASpaceSmithCharacter::OnFire()
{
	if (CharacterController->GetInventoryVisible())
		return;

	if (CharacterController->GetViewportWidgetVisible())
		return;

	if (SlotItem)
	{
		SlotItem->Fire();
	}
}

void ASpaceSmithCharacter::OnFireEnd()
{
	if (SlotItem)
	{
		SlotItem->FireEnd();
	}
}

void ASpaceSmithCharacter::OnHold()
{
	if (CharacterController->GetInventoryVisible())
		return;

	if (CharacterController->GetViewportWidgetVisible())
		return;

	if (HasHoldingPickable())
	{
		Drop();
	}
	else if (HasSelectable())
	{
		PickUp();
	}
}

void ASpaceSmithCharacter::OnAction()
{
	if (CharacterController->GetInventoryVisible())
		return;

	if (CharacterController->GetViewportWidgetVisible())
		return;

	if (HasSelectable())
	{
		if (IAction* Actionable = Cast<IAction>(Selectable.GetObject()))
		{
			IAction::Execute_Action(Selectable.GetObject());
		}
	}
}

void ASpaceSmithCharacter::OnInteract()
{
	if (CharacterController->GetInventoryVisible())
		return;

	if (HasSelectable())
	{
		if (IInteract* Interactable = Cast<IInteract>(Selectable.GetObject()))
		{
			IInteract::Execute_Interact(Selectable.GetObject(), CharacterController);

			if (AActor* InteractableActor = Cast<AActor>(Selectable.GetObject()))
			{
				if (InteractableActor->IsActorBeingDestroyed())
				{
					ResetHolding();
				}
			}
		}
	}
}

void ASpaceSmithCharacter::OnInventory()
{
	if (CharacterController->GetViewportWidgetVisible() && !CharacterController->GetInventoryVisible())
		return;

	CharacterController->ToggleInventoryUMG();
}

void ASpaceSmithCharacter::Slot(UInventorySlot* Slot)
{
	if (SlotItem)
	{
		SlotItem->FireEnd();
		SlotItem->Destroy();
	}

	if (Slot)
	{
		SlotItem = GetWorld()->SpawnActor<ABaseItem>(Slot->Row.Class, GetActorLocation(), GetActorRotation());
		if (SlotItem)
		{
			SlotItem->Initialize(Slot->Row);
			SlotItem->SetOwnerController(CharacterController);
			SlotItem->SetActorEnableCollision(false);
			SlotItem->DisableComponentsSimulatePhysics();
			SlotItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("AttachPoint"));
		}
	}
}

void ASpaceSmithCharacter::SlotKey1()
{
	Slot(CharacterController->SelectSlot(0));
}

void ASpaceSmithCharacter::SlotKey2()
{
	Slot(CharacterController->SelectSlot(1));
}

void ASpaceSmithCharacter::SlotKey3()
{
	Slot(CharacterController->SelectSlot(2));
}

void ASpaceSmithCharacter::SlotKey4()
{
	Slot(CharacterController->SelectSlot(3));
}

void ASpaceSmithCharacter::SlotKey5()
{
	Slot(CharacterController->SelectSlot(4));
}

void ASpaceSmithCharacter::SlotKey6()
{
	Slot(CharacterController->SelectSlot(5));
}

void ASpaceSmithCharacter::SlotKey7()
{
	Slot(CharacterController->SelectSlot(6));
}

void ASpaceSmithCharacter::SlotKey8()
{
	Slot(CharacterController->SelectSlot(7));
}

void ASpaceSmithCharacter::SlotKey9()
{
	Slot(CharacterController->SelectSlot(8));
}

void ASpaceSmithCharacter::SlotKey0()
{
	Slot(CharacterController->SelectSlot(9));
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
	PlayerInputComponent->BindAxis("Turn", this, &ASpaceSmithCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpaceSmithCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASpaceSmithCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASpaceSmithCharacter::OnFireEnd);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASpaceSmithCharacter::OnInteract);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &ASpaceSmithCharacter::OnAction);
	PlayerInputComponent->BindAction("Hold", IE_Pressed, this, &ASpaceSmithCharacter::OnHold);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ASpaceSmithCharacter::OnInventory);

	PlayerInputComponent->BindAction("1", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey1);
	PlayerInputComponent->BindAction("2", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey2);
	PlayerInputComponent->BindAction("3", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey3);
	PlayerInputComponent->BindAction("4", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey4);
	PlayerInputComponent->BindAction("5", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey5);
	PlayerInputComponent->BindAction("6", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey6);
	PlayerInputComponent->BindAction("7", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey7);
	PlayerInputComponent->BindAction("8", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey8);
	PlayerInputComponent->BindAction("9", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey9);
	PlayerInputComponent->BindAction("0", IE_Pressed, this, &ASpaceSmithCharacter::SlotKey0);
}

void ASpaceSmithCharacter::MoveForward(float Value)
{
	if (CharacterController->GetInventoryVisible())
		return;

	if (CharacterController->GetViewportWidgetVisible())
		return;

	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASpaceSmithCharacter::MoveRight(float Value)
{
	if (CharacterController->GetInventoryVisible())
		return;

	if (CharacterController->GetViewportWidgetVisible())
		return;

	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASpaceSmithCharacter::AddControllerYawInput(float Val)
{
	if (CharacterController->GetInventoryVisible())
		return;

	if (CharacterController->GetViewportWidgetVisible())
		return;

	Super::AddControllerYawInput(Val);
}

void ASpaceSmithCharacter::AddControllerPitchInput(float Val)
{
	if (CharacterController->GetInventoryVisible())
		return;

	if (CharacterController->GetViewportWidgetVisible())
		return;

	Super::AddControllerPitchInput(Val);
}