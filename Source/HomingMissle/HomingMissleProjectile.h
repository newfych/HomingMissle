// (Hyd-ra).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HomingMissleProjectile.generated.h"


class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;

UCLASS(config=Game)
class AHomingMissleProjectile : public AActor
{
	GENERATED_BODY()
	
	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	AHomingMissleProjectile();

	/** Setters */
	FORCEINLINE void SetTargetPoint(const FVector& EndPoint) { TargetPoint = EndPoint; }
	FORCEINLINE void SetStaticMesh(UStaticMesh* ProjectileMesh) { Mesh = ProjectileMesh; }
	FORCEINLINE void SetMeshScale(const FVector& Scale) { MeshScale = Scale; }
	FORCEINLINE void SetSound(USoundBase* ProjectileSound) { Sound = ProjectileSound; }
	FORCEINLINE void SetNiagaraSystem(UNiagaraSystem* ProjectileNiagaraSystem) { NiagaraSystem = ProjectileNiagaraSystem; }
	
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	virtual void BeginPlay() override;
	

private:
	/** Static Mesh Variables*/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY()
	UStaticMesh* Mesh;
	UPROPERTY()
	FVector MeshScale;

	/** Sound Variables*/
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponent;
	UPROPERTY()
	USoundBase* Sound;
	
	/** Niagara System Variables*/
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComponent;
	UPROPERTY()
	UNiagaraSystem* NiagaraSystem;

	/** Target Point*/
	FVector TargetPoint;

	/**
	 * Functions 
	 */
	/** Setup Components */
	void SetupComponents() const;
};

