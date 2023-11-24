// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticAttack.h"

ADamageActorStaticAttack::ADamageActorStaticAttack()
{
	InitialLifeSpan = 0.3f;
}

float ADamageActorStaticAttack::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner || Histroy.Contains(Who)) return 0.0f;

	ShouldDestroy = true;

	return Super::Damage(Who, DeltaTime) + DamageScale * 260.0f;
}