// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticSparrowQ.h"

void ADamageActorStaticSparrowQ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Disabled && SumTime > 5.4f)
	{
		auto P = GetComponentByClass(UParticleSystemComponent::StaticClass());
		P->Deactivate();

		Disabled = true;
	}

	if (SumTime > 7.0f)
	{
		ShouldDestroy = true;
	}
}

float ADamageActorStaticSparrowQ::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner) return 0.0f;

	return Super::Damage(Who, DeltaTime) + 100.0f * DamageScale * DeltaTime;
}