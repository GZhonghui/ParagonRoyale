// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticFengMaoE.h"

ADamageActorStaticFengMaoE::ADamageActorStaticFengMaoE()
{
	InitialLifeSpan = 2.8f; // Useless
}

void ADamageActorStaticFengMaoE::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorStaticFengMaoE::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Disabled && SumTime > 2.4f)
	{
		auto P = GetComponentByClass(UParticleSystemComponent::StaticClass());
		P->Deactivate();

		Disabled = true;
	}
	if (SumTime > 2.6f)
	{
		ShouldDestroy = true;
	}
}

float ADamageActorStaticFengMaoE::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner) return 0.0f;
	return Super::Damage(Who, DeltaTime) + 400.0f * DamageScale * DeltaTime;
}