// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticAuroraQIce.h"

ADamageActorStaticAuroraQIce::ADamageActorStaticAuroraQIce()
{
	InitialLifeSpan = 5.0f;
}

float ADamageActorStaticAuroraQIce::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner) return 0.0f;

	return Super::Damage(Who, DeltaTime) + 120.0f * DamageScale * DeltaTime;
}