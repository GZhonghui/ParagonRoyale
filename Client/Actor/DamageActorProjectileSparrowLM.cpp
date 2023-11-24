// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorProjectileSparrowLM.h"

ADamageActorProjectileSparrowLM::ADamageActorProjectileSparrowLM()
{
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
}

void ADamageActorProjectileSparrowLM::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorProjectileSparrowLM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADamageActorProjectileSparrowLM::Damage(AActor* Who, float DeltaTime)
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