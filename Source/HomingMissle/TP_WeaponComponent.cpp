// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "HomingMissleCharacter.h"
#include "HomingMissleProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
/*
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}
*/


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			World->SpawnActor<AHomingMissleProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::Aim()
{
	if (Character == nullptr || Character->GetController() == nullptr || !GetWorld())
	{
		return;
	}

	const auto PlayerController = GetPlayerController();

	if (!PlayerController) return;

	const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector ShootDirection = ViewRotation.Vector();
	const FVector TraceEnd = SpawnLocation + ShootDirection * TraceMaxDistance;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation, TraceEnd, ECollisionChannel::ECC_Visibility);

	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();
		FString HitActorName = HitActor->GetActorLabel();

		if (HitActor->CanBeDamaged()) {
			UE_LOG(LogTemp, Warning, TEXT("CAN BE: %s"), *HitActorName);
			if (CurrentTarget)
			{
				SetCustomDepthForActor(CurrentTarget, false);
			}
			SetCustomDepthForActor(HitActor, true);
			CurrentTarget = HitActor;
		}
	}
}

void UTP_WeaponComponent::PreviousAmmo()
{
}

void UTP_WeaponComponent::NextAmmo()
{
}

void UTP_WeaponComponent::AttachWeapon(AHomingMissleCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
			// Aim (Hyd-ra)
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Aim);
			// Previous Ammo (Hyd-ra)
			EnhancedInputComponent->BindAction(PrevAmmoAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::PreviousAmmo);
			// Next Ammo (Hyd-ra)
			EnhancedInputComponent->BindAction(NextAmmoAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::NextAmmo);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

APlayerController* UTP_WeaponComponent::GetPlayerController()
{
	if (!Character) return nullptr;

	return Cast<APlayerController>(Character->GetController());
}

void UTP_WeaponComponent::SetCustomDepthForActor(AActor* Actor, bool bEnableCustomDepth)
{
	if (!Actor) return;

	// Get all mesh components attached to the actor
	TArray<UMeshComponent*> MeshComponents;
	Actor->GetComponents(MeshComponents);

	// Iterate through all mesh components
	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		// Enable or disable custom depth rendering for the mesh component
		MeshComponent->SetRenderCustomDepth(bEnableCustomDepth);
	}
}
