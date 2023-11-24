// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticGruxQ.h"

ADamageActorStaticGruxQ::ADamageActorStaticGruxQ()
{

}

void ADamageActorStaticGruxQ::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorStaticGruxQ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Disabled && SumTime > 2.0f)
	{
		auto P = GetComponentByClass(UParticleSystemComponent::StaticClass());
		if (P)
		{
			P->Deactivate();
		}

		Disabled = true;
	}

	if (SumTime > 4.0f)
	{
		ShouldDestroy = true;
	}
}

float ADamageActorStaticGruxQ::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner) return 0.0f;

	return Super::Damage(Who, DeltaTime) + 200.0f * DamageScale * DeltaTime;
}