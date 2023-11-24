// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorProjectileSparrowRM.h"

ADamageActorProjectileSparrowRM::ADamageActorProjectileSparrowRM()
{
	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->MaxSpeed = 1200.0f;
}

void ADamageActorProjectileSparrowRM::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorProjectileSparrowRM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADamageActorProjectileSparrowRM::Damage(AActor* Who, float DeltaTime)
{
	if (Owner == Who || Histroy.Contains(Who)) return 0.0f;

	float Ret = Super::Damage(Who, DeltaTime);

	Ret += 300.0f * DamageScale;

	if (ParticleOnDestroy)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleOnDestroy, GetActorTransform(), true);
	}

	ShouldDestroy = true;

	return Ret;
}