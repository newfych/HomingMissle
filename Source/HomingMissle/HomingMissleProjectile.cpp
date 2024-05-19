//(Hyd-ra).

#include "HomingMissleProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"

AHomingMissleProjectile::AHomingMissleProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	SetRootComponent(CollisionComp);
	
	CollisionComp->OnComponentHit.AddDynamic(this, &AHomingMissleProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	//CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	//CollisionComp->CanCharacterStepUpOn = ECB_No;
	
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	//ProjectileMovement->UpdatedComponent = CollisionComp;
	//ProjectileMovement->InitialSpeed = 3000.f;
	//ProjectileMovement->MaxSpeed = 3000.f;
	//ProjectileMovement->bRotationFollowsVelocity = true;
	//ProjectileMovement->bShouldBounce = true;*/

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMeshComponent"));
	MeshComponent->SetupAttachment(CollisionComp);
	
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MyAudioComponent"));
	AudioComponent->SetupAttachment(CollisionComp);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MyNiagaraComponent"));
	NiagaraComponent->SetupAttachment(CollisionComp);
	
	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AHomingMissleProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetupComponents();
	
}

void AHomingMissleProjectile::SetupComponents() const
{
	if (Mesh)
	{
		MeshComponent->SetStaticMesh(Mesh);
		MeshComponent->SetRelativeScale3D(MeshScale);
	}
	if (Sound)
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
	if (NiagaraSystem)
	{
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->Activate();
	}
}

void AHomingMissleProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}


