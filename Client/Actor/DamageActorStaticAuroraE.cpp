// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticAuroraE.h"

ADamageActorStaticAuroraE::ADamageActorStaticAuroraE()
{
	InitialLifeSpan = 8.0f;
}

void ADamageActorStaticAuroraE::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorStaticAuroraE::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADamageActorStaticAuroraE::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner) return 0.0f;

	return Super::Damage(Who, DeltaTime) + DeltaTime * 80.0f * DamageScale;
}