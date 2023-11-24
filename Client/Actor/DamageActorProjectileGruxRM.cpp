// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorProjectileGruxRM.h"

ADamageActorProjectileGruxRM::ADamageActorProjectileGruxRM()
{
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 1500.0f;
}

void ADamageActorProjectileGruxRM::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorProjectileGruxRM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADamageActorProjectileGruxRM::Damage(AActor* Who, float DeltaTime)
{
	if (Owner == Who || Histroy.Contains(Who)) return 0.0f;

	float Ret = Super::Damage(Who, DeltaTime);

	Ret += 500.0f;

	if (ParticleOnDestroy)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleOnDestroy, GetActorTransform(), true);
	}

	ShouldDestroy = true;

	return Ret;
}