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

	/** Interpolation Speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float InterpolationSpeed;

	/** Projectile Speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float Speed { 1500.f };

	/** Rotation Speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RotateSpeed { 1.f };

	/** Prediction Variables  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prediction", meta = (AllowPrivateAccess = "true"))
	float MaxDistancePredict { 100.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prediction", meta = (AllowPrivateAccess = "true"))
	float MinDistancePredict { 5.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prediction", meta = (AllowPrivateAccess = "true"))
	float MaxTimePrediction { 5.f };

	/** Deviation Variables  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deviation", meta = (AllowPrivateAccess = "true"))
	float DeviationAmount { 2.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deviation", meta = (AllowPrivateAccess = "true"))
	float DeviationSpeed { 2.f };

	/** Setters */
	FORCEINLINE void SetTargetPoint(const FVector& EndPoint) { TargetPoint = EndPoint; }
	FORCEINLINE void SetTargetActor(AActor* Actor) { TargetActor = Actor; }
	FORCEINLINE void SetIsHoming(const bool IsHomingProjectile) { IsHoming = IsHomingProjectile; }
	FORCEINLINE void SetName(const FString& AmmoName) { Name = AmmoName; }
	FORCEINLINE void SetStaticMesh(UStaticMesh* ProjectileMesh) { Mesh = ProjectileMesh; }
	FORCEINLINE void SetSpeed(float ProjectileSpeed) { Speed = ProjectileSpeed; }
	FORCEINLINE void SetMeshScale(const FVector& Scale) { MeshScale = Scale; }
	FORCEINLINE void SetSound(USoundBase* ProjectileSound) { Sound = ProjectileSound; }
	FORCEINLINE void SetNiagaraSystem(UNiagaraSystem* ProjectileNiagaraSystem) { NiagaraSystem = ProjectileNiagaraSystem; }
	
	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	
protected:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	
private:
	/** Name */
	UPROPERTY()
	FString Name;
	
	/** Static Mesh Variables */
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY()
	UStaticMesh* Mesh;
	UPROPERTY()
	FVector MeshScale;

	/** Sound Variables */
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponent;
	UPROPERTY()
	USoundBase* Sound;
	
	/** Niagara System Variables */
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComponent;
	UPROPERTY()
	UNiagaraSystem* NiagaraSystem;

	/** Target Point */
	FVector TargetPoint;

	/** Target Actor */
	UPROPERTY()
	AActor* TargetActor;

	/** Target Point */
	bool IsHoming;
	
	/** Homing Variables */
	FVector StandardPrediction;
	FVector DeviatedPrediction;

	// Helper methods
	void PredictMovement(float LeadTimePercentage);
	void AddDeviation(float LeadTimePercentage);
	void RotateMissile();
	/**
	 * Functions 
	 */
	/** Setup Components */
	void SetupComponents() const;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

