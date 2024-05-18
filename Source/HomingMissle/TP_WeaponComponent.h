// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "NiagaraSystem.h"

#include "TP_WeaponComponent.generated.h"

USTRUCT(BlueprintType)
struct FAmmoDataTable : public FTableRowBase
{
	GENERATED_BODY()

	/** Projectile Name (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	FString Name;

	/** Projectile Static Mesh (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UStaticMesh* ProjectileMesh;

	/** Projectile Mesh Scale (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	FVector ProjectileMeshScale{ (1.f, 1.f, 1.f) };

	/** Projectile Particle System (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UNiagaraSystem* ProjectileEffect;

	/** Projectile Sound  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	USoundBase* ProjectileSound;

	/** Icon Image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UTexture2D* AmmoIcon;

	/** Crosshair Image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UTexture2D* CrosshairImage;
};

class AHomingMissleCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMINGMISSLE_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AHomingMissleProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AHomingMissleCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	AHomingMissleCharacter* Character;
};
