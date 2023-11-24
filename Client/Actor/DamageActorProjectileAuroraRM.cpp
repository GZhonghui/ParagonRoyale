// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorProjectileAuroraRM.h"

ADamageActorProjectileAuroraRM::ADamageActorProjectileAuroraRM()
{
	ProjectileMovement->InitialSpeed = 1800.0f;
	ProjectileMovement->MaxSpeed = 1800.0f;
}

void ADamageActorProjectileAuroraRM::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorProjectileAuroraRM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


float ADamageActorProjectileAuroraRM::Damage(AActor* Who, float DeltaTime)
{
	if (Owner == Who || Histroy.Contains(Who)) return 0.0f;

	float Ret = Super::Damage(Who, DeltaTime);

	Ret += 300.0f;

	if (ParticleOnDestroy)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleOnDestroy, GetActorTransform(), true);
	}

	ShouldDestroy = true;

	return Ret;
}