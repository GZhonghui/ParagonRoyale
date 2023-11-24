// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticAuroraSP.h"

ADamageActorStaticAuroraSP::ADamageActorStaticAuroraSP()
{
	InitialLifeSpan = 6.5f;
}

void ADamageActorStaticAuroraSP::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorStaticAuroraSP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HeroAurora && IsValid(HeroAurora))
	{
		SetActorLocation(HeroAurora->GetActorLocation());
	}
}

float ADamageActorStaticAuroraSP::Damage(AActor* Who, float DeltaTime)
{
	return Super::Damage(Who, DeltaTime);
}