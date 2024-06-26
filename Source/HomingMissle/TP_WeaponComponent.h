// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "NiagaraSystem.h"
#include "WeaponWidget.h"

#include "TP_WeaponComponent.generated.h"

USTRUCT(BlueprintType)
struct FAmmoDataTable : public FTableRowBase
{
	GENERATED_BODY()

	/** Projectile Name (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	FString Name;

	/** Is Homing Projectile (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool IsHomingProjectile { false };

	/** Is Homing Projectile (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float ProjectileSpeed { 2000.f};

	/** Projectile Static Mesh (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UStaticMesh* ProjectileMesh;

	/** Projectile Mesh Scale (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	FVector ProjectileMeshScale { (1.f, 1.f, 1.f)};

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
	FVector MuzzleOffset { (100.0f, 0.0f, 10.0f)};

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Aim Input Action (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	/** Previous Ammo Action (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrevAmmoAction;

	/** Next Ammo Action (Hyd-ra)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* NextAmmoAction;

	/** Sets default values for this component's properties */
	//UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AHomingMissleCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	/** Make the weapon Aim to Target (Hyd-ra)*/
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Aim();

	/** Switch To Previous Ammo (Hyd-ra)*/
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PreviousAmmo();

	/** Switch To Next Ammo (Hyd-ra)*/
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void NextAmmo();
	

protected:
	
	/** Max Weapon Trace Distance (Hyd-ra)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float TraceMaxDistance{ 5000.f };

	/** Muzzle Socket Name (Hyd-ra)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName SocketName { "Muzzle" };

	/** Data Table for Ammo Properties (Hyd-ra)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UDataTable* AmmoDataTable;

	/** Ammo Panel Widget Class (Hyd-ra)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> AmmoPanelWidgetClass;

	/**
	* Functions (Hyd-ra)
	*/
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	UPROPERTY()
	AHomingMissleCharacter* Character;

	/** Index of Current Ammo (Hyd-ra)*/
	int32 CurrentAmmoIndex{ 0 };

	/** Ammo Types Count (Hyd-ra)*/
	int32 AmmoCount{ 0 };
	
	/** Current Target Actor (Hyd-ra)*/
	UPROPERTY()
	AActor* CurrentTarget;

	/** Data Table Rows Names (Hyd-ra)*/
	TArray<FName> RowNames;

	/** Data Table Rows Names (Hyd-ra)*/
	TArray<FString> AmmoNames;

	/** Data Table Rows Names (Hyd-ra)*/
	TArray<bool> IsHomingBooleans;

	/** Data Table Rows Names (Hyd-ra)*/
	TArray<float> SpeedArray;

	/** Weapon Widget (Hyd-ra)*/
	UPROPERTY()
	UWeaponWidget* WeaponWidget;

	/** Weapon Meshes Array (Hyd-ra)*/
	UPROPERTY()
	TArray<UStaticMesh*> AmmoMeshes;

	/** Meshes Scales Array (Hyd-ra)*/
	UPROPERTY()
	TArray<FVector> MeshesScales;

	/** Sounds Array (Hyd-ra)*/
	UPROPERTY()
	TArray<USoundBase*> Sounds;

	/** Particles Array (Hyd-ra)*/
	UPROPERTY()
	TArray<UNiagaraSystem*> Particles;

	/** WeaponIcons Array (Hyd-ra)*/
	UPROPERTY()
	TArray<UTexture2D*> AmmoIcons;

	/** Current CrossHair Image (Hyd-ra)*/
	UPROPERTY()
	TArray<UTexture2D*> CrossHairs;

	/**
	 * Functions (Hyd-ra)
	 */
	/** Helper Function To Get Player Controller (Hyd-ra)*/
	APlayerController* GetPlayerController() const;

	/** Set Custom Depth to Outline Actor (Hyd-ra)*/
	static void SetCustomDepthForActor(const AActor* Actor, bool bEnableCustomDepth);

	/** Retrieve Data from Data Table (Hyd-ra)*/
	void RetrieveRowsFromDataTable();

	/** Retrieve Data Table Row Data (Hyd-ra)*/
	void GetRowData(const FName& RowName);

	/** Create Weapon Widget (Hyd-ra)*/
	void CreateWeaponWidget();

	/** Update Weapon Widget (Hyd-ra)*/
	void UpdateWeaponWidget();

	/** Get Trace Hit Result (Hyd-ra)*/
	FHitResult GetHitResult() const;

	/** Calculate Trace End (Hyd-ra)*/
	FVector CalculateTraceEnd() const;

	/** Get Muzzle Socket World Location (Hyd-ra)*/
	FVector GetSocketWorldLocation() const;
};
