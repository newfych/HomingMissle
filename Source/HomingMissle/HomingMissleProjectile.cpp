//(Hyd-ra).

#include "HomingMissleProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

AHomingMissleProjectile::AHomingMissleProjectile() 
{
	PrimaryActorTick.bCanEverTick = true;
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	SetRootComponent(CollisionComp);

	// set up a notification for when this component hits something blocking
	//CollisionComp->OnComponentHit.AddDynamic(this, &AHomingMissleProjectile::OnHit);		
	
	// Messh Component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMeshComponent"));
	MeshComponent->SetupAttachment(CollisionComp);
	MeshComponent->BodyInstance.bNotifyRigidBodyCollision = true;
	MeshComponent->OnComponentHit.AddDynamic(this, &AHomingMissleProjectile::OnHit);	

	// Audio Component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MyAudioComponent"));
	AudioComponent->SetupAttachment(CollisionComp);

	// Niagara System Component
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MyNiagaraComponent"));
	NiagaraComponent->SetupAttachment(CollisionComp);

	InitialLifeSpan = 6.0f;
}

void AHomingMissleProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetupComponents();
}

void AHomingMissleProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsHoming)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector TargetLocation = TargetPoint;

		float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);

		float LeadTimePercentage = FMath::Clamp((DistanceToTarget - MinDistancePredict) / (MaxDistancePredict - MinDistancePredict), 0.0f, 1.0f);

		PredictMovement(LeadTimePercentage);
		AddDeviation(LeadTimePercentage);
		RotateMissile();

		FVector ForwardVector = GetActorForwardVector();
		FVector NewLocation = CurrentLocation + (ForwardVector * Speed * DeltaTime);

		// Update Actor Location
		SetActorLocation(NewLocation);
	}
	else
	{
		FVector Direction = (TargetPoint - GetActorLocation()).GetSafeNormal();
		FVector NewLocation = GetActorLocation() + (Direction * Speed * DeltaTime);

		// Update Actor Location
		SetActorLocation(NewLocation);
	}
}

void AHomingMissleProjectile::PredictMovement(float LeadTimePercentage)
{
	FVector TargetVelocity = TargetActor->GetVelocity();
	float PredictionTime = FMath::Lerp(0.0f, MaxTimePrediction, LeadTimePercentage);
	StandardPrediction = TargetPoint + (TargetVelocity * PredictionTime);
}

void AHomingMissleProjectile::AddDeviation(float LeadTimePercentage)
{
	FVector Deviation = FVector(FMath::Cos(GetWorld()->GetTimeSeconds() * DeviationSpeed), FMath::Sin(GetWorld()->GetTimeSeconds() * DeviationSpeed), 0);
	FVector PredictionOffset = GetTransform().TransformVector(Deviation) * DeviationAmount * LeadTimePercentage;
	DeviatedPrediction = StandardPrediction + PredictionOffset;
}

void AHomingMissleProjectile::RotateMissile()
{
	FVector Heading = DeviatedPrediction - GetActorLocation();
	FRotator TargetRotation = Heading.Rotation();
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), RotateSpeed);
	SetActorRotation(NewRotation);
}

void AHomingMissleProjectile::SetupComponents() const
{
	if (Mesh)
	{
		MeshComponent->SetStaticMesh(Mesh);
		MeshComponent->SetRelativeScale3D(MeshScale);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Mesh For %s!"), *Name);
	}
	if (Sound)
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Sound Asset For %s!"), *Name);
	}
	if (NiagaraSystem)
	{
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->Activate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Niagara System For %s!"), *Name);
	}
}

void AHomingMissleProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Destroy Projectile
	this->K2_DestroyActor();
}


