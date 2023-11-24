// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticSparrowSP.h"

void ADamageActorStaticSparrowSP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Disabled && SumTime > 7.0f)
	{
		auto P = GetComponentByClass(UParticleSystemComponent::StaticClass());
		P->Deactivate();

		Disabled = true;
	}

	if (SumTime > 8.0f)
	{
		Destroy();
	}
}

float ADamageActorStaticSparrowSP::Damage(AActor* Who, float DeltaTime)
{
	Super::Damage(Who, DeltaTime);

	if (Who == Owner)
	{
		return DeltaTime * -100.0f;
	}

	return 0.0f;
}