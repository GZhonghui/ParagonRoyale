// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticFengMaoRM.h"

#include "../Character/HeroCharacterBase.h"

ADamageActorStaticFengMaoRM::ADamageActorStaticFengMaoRM()
{

}

void ADamageActorStaticFengMaoRM::BeginPlay()
{
	Super::BeginPlay();
}


void ADamageActorStaticFengMaoRM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HeroFengMao && IsValid(HeroFengMao))
	{
		auto HeroLocation = HeroFengMao->GetActorLocation();

		auto Hero = Cast<AHeroCharacterBase>(HeroFengMao);
		if (Hero)
		{
			HeroLocation.X = Hero->ShadowLocation.X;
			HeroLocation.Y = Hero->ShadowLocation.Y;
		}

		SetActorLocation(HeroLocation);
	}

	if (SumTime > 6.0f)
	{
		ShouldDestroy = true;
	}
}

float ADamageActorStaticFengMaoRM::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner) return 0.0f;

	return Super::Damage(Who, DeltaTime) + 150.0f * DamageScale * DeltaTime;
}