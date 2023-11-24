// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticSparrowE.h"

ADamageActorStaticSparrowE::ADamageActorStaticSparrowE()
{

}

void ADamageActorStaticSparrowE::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> AP;
	GetComponents(UParticleSystemComponent::StaticClass(), AP);

	for (int i = 0; i < AP.Num(); ++i)
	{
		P.Add(Cast<UParticleSystemComponent>(AP[i]));
	}
	Collision = Cast<UCapsuleComponent>(GetComponentByClass(UCapsuleComponent::StaticClass()));
}

void ADamageActorStaticSparrowE::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float S = std::min(2.4f, 1.0f + SumTime / 3.2f);

	for (int i = 0; i < P.Num(); i += 1)
	{
		P[i]->SetWorldScale3D(FVector(S, S, S));

		if (SumTime > 6.4f)
		{
			P[i]->Deactivate();
		}
	}

	if (Collision)
	{
		Collision->SetWorldScale3D(FVector(S, S, S));
	}

	if (SumTime > 6.8f)
	{
		ShouldDestroy = true;
	}
}

float ADamageActorStaticSparrowE::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner) return 0.0f;

	return Super::Damage(Who, DeltaTime) + 80.0f * DamageScale * DeltaTime;
}