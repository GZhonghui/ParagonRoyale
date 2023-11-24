// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticFengMaoQ.h"

ADamageActorStaticFengMaoQ::ADamageActorStaticFengMaoQ()
{

}

void ADamageActorStaticFengMaoQ::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorStaticFengMaoQ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Disabled && SumTime > 5.0f)
	{
		TArray<UActorComponent*> AP;
		GetComponents(UParticleSystemComponent::StaticClass(), AP);

		for (int i = 0; i < AP.Num(); ++i)
		{
			auto P = Cast<UParticleSystemComponent>(AP[i]);
			if (P)
			{
				P->Deactivate();
			}
		}

		Disabled = true;
	}

	if (SumTime > 6.0f)
	{
		ShouldDestroy = true;
	}
}

float ADamageActorStaticFengMaoQ::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner) return 0.0f;
	return Super::Damage(Who, DeltaTime) + 400.0f * DamageScale * DeltaTime;
}