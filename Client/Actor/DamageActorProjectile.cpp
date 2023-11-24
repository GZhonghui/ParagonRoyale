// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorProjectile.h"

ADamageActorProjectile::ADamageActorProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(32.0f);
	// CollisionComp->OnComponentHit.AddDynamic(this, &ABulletActor::OnHit);
	// CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletActor::OnOverlapBegin);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->MaxSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Die after 5 seconds by default
	InitialLifeSpan = 5.0f;
}

// Called when the game starts or when spawned
void ADamageActorProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADamageActorProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADamageActorProjectile::Damage(AActor* Who, float DeltaTime)
{
	return Super::Damage(Who, DeltaTime);
}