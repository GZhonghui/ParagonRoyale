// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorPoison.h"

ADamageActorPoison::ADamageActorPoison()
{
	InitialLifeSpan = 600.0f;
}

void ADamageActorPoison::BeginPlay()
{
	Super::BeginPlay();

	PlayerID = 0;

	InitLocation = GetActorLocation();
}

void ADamageActorPoison::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// AddActorLocalOffset(FVector(MoveSpeed * DeltaTime, 0.0f, 0.0f));
	if (GI)
	{
		auto Forward = GetActorForwardVector();
		auto GameTime = GI->GetGameTime();
		SetActorLocation(InitLocation + Forward * MoveSpeed * GameTime);
	}
}

float ADamageActorPoison::Damage(AActor* Who, float DeltaTime)
{
	float Res = Super::Damage(Who, DeltaTime);

	return Res + DeltaTime * DamagePerSecond;
}