// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "HomingMissleCharacter.h"
#include "HomingMissleProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr) return;

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		if (UWorld* const World = GetWorld(); World != nullptr)
		{
			const auto PlayerController = GetPlayerController();
			if (!PlayerController) return;
			
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			const FVector SpawnLocation = GetSocketWorldLocation();

			FHitResult HitResult = GetHitResult();
			FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : CalculateTraceEnd();

			FTransform SpawnTransform(SpawnRotation, SpawnLocation);
			AHomingMissleProjectile* Projectile = World->SpawnActorDeferred<AHomingMissleProjectile>(ProjectileClass, SpawnTransform);

			bool IsHoming = IsHomingBooleans[CurrentAmmoIndex];
			if (IsHoming && CurrentTarget !=nullptr)
			{
				EndPoint = CurrentTarget->GetActorLocation();
			}
			else
			{
				IsHoming = false;
			}
			
			if (Projectile)
			{
				Projectile->SetName(AmmoNames[CurrentAmmoIndex]);
				Projectile->SetStaticMesh(AmmoMeshes[CurrentAmmoIndex]);
				Projectile->SetTargetActor(CurrentTarget);
				Projectile->SetIsHoming(IsHoming);
				Projectile->SetSpeed(SpeedArray[CurrentAmmoIndex]);
				Projectile->SetMeshScale(MeshesScales[CurrentAmmoIndex]);
				Projectile->SetSound(Sounds[CurrentAmmoIndex]);
				Projectile->SetNiagaraSystem(Particles[CurrentAmmoIndex]);
				Projectile->SetTargetPoint( EndPoint );
				Projectile->FinishSpawning(SpawnTransform);

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
		}
	}
}

/** Make the weapon Aim to Target (Hyd-ra)*/
void UTP_WeaponComponent::Aim()
{
	if (Character == nullptr || Character->GetController() == nullptr || !GetWorld())
	{
		return;
	}

	FHitResult HitResult = GetHitResult();

	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor->CanBeDamaged()) {
			if (CurrentTarget)
			{
				SetCustomDepthForActor(CurrentTarget, false);
			}
			SetCustomDepthForActor(HitActor, true);
			CurrentTarget = HitActor;
		}
	}
}

/** Switch To Previous Ammo (Hyd-ra)*/
void UTP_WeaponComponent::PreviousAmmo()
{
	if (AmmoCount <= 0) return;
	CurrentAmmoIndex -= 1;
	if (CurrentAmmoIndex < 0)
	{
		CurrentAmmoIndex = AmmoCount - 1;
	}
	UpdateWeaponWidget();
}

/** Switch To Next Ammo (Hyd-ra)*/
void UTP_WeaponComponent::NextAmmo()
{
	if (AmmoCount <= 0) return;
	CurrentAmmoIndex += 1;
	if (CurrentAmmoIndex > (AmmoCount - 1))
	{
		CurrentAmmoIndex = 0;
	}
	UpdateWeaponWidget();
}

/**  Attach Weapon*/
void UTP_WeaponComponent::AttachWeapon(AHomingMissleCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
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

	RetrieveRowsFromDataTable();
	CreateWeaponWidget();
}

/**  End Play*/
void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

/** Helper Function To Get Player Controller (Hyd-ra)*/
APlayerController* UTP_WeaponComponent::GetPlayerController() const
{
	if (!Character) return nullptr;

	return Cast<APlayerController>(Character->GetController());
}

/**  Set Custom Depth For Outline Effect(Hyd-ra)*/
void UTP_WeaponComponent::SetCustomDepthForActor(const AActor* Actor, const bool bEnableCustomDepth)
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

/**  Retrieve Data From DataTable (Hyd-ra)*/
void UTP_WeaponComponent::RetrieveRowsFromDataTable()
{
	if (!AmmoDataTable) return;

	RowNames = AmmoDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		GetRowData(RowName);
	}
	AmmoCount = RowNames.Num();
}

/** Retrieve Data Table Row Data (Hyd-ra)*/
void UTP_WeaponComponent::GetRowData(const FName& RowName)
{
	{
		if (const FAmmoDataTable* AmmoDataRow = AmmoDataTable->FindRow<FAmmoDataTable>(FName(RowName), TEXT("")))
		{
			AmmoMeshes.Add(AmmoDataRow->ProjectileMesh);
			AmmoNames.Add(AmmoDataRow->Name);
			IsHomingBooleans.Add(AmmoDataRow->IsHomingProjectile);
			SpeedArray.Add(AmmoDataRow->ProjectileSpeed);
			MeshesScales.Add(AmmoDataRow->ProjectileMeshScale);
			Sounds.Add(AmmoDataRow->ProjectileSound);
			Particles.Add(AmmoDataRow->ProjectileEffect);
			AmmoIcons.Add(AmmoDataRow->AmmoIcon);
			CrossHairs.Add(AmmoDataRow->CrosshairImage);
		}
	}
}

/**  Create Weapon Widget (Hyd-ra)*/
void UTP_WeaponComponent::CreateWeaponWidget()
{
	if (!AmmoPanelWidgetClass) return;
	WeaponWidget = CreateWidget<UWeaponWidget>(GetWorld(), AmmoPanelWidgetClass);
	if (WeaponWidget)
	{
		WeaponWidget->AddToViewport();
	}
	WeaponWidget->AddIconsToPanel(AmmoIcons);
	UpdateWeaponWidget();
}

/**  Update Weapon Widget (Hyd-ra)*/
void UTP_WeaponComponent::UpdateWeaponWidget()
{
	WeaponWidget->SetCrosshairImage(CrossHairs[CurrentAmmoIndex]);
	WeaponWidget->SetActiveIcon(CurrentAmmoIndex);
}

/**  Get Line Trace Hit Result (Hyd-ra)*/
FHitResult UTP_WeaponComponent::GetHitResult() const
{
	FHitResult HitResult;
	
	
	GetWorld()->LineTraceSingleByChannel(HitResult, GetSocketWorldLocation(), CalculateTraceEnd(), ECollisionChannel::ECC_Visibility);
	return HitResult;
}

/**  Calculate Trace End Point for Line Trace (Hyd-ra)*/
FVector UTP_WeaponComponent::CalculateTraceEnd() const
{
	FVector TraceEnd;
	const auto PlayerController = GetPlayerController();
	if (!PlayerController) return TraceEnd;
	
	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	
	const FVector ShootDirection = ViewRotation.Vector();
	TraceEnd = GetSocketWorldLocation() + ShootDirection * TraceMaxDistance;

	return TraceEnd;
}

/**  Get Muzzle Socket Location (Hyd-ra)*/
FVector UTP_WeaponComponent::GetSocketWorldLocation() const
{
	return this->GetSocketLocation(SocketName);
}
