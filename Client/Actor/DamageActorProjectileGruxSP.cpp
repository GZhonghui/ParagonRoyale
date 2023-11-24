// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorProjectileGruxSP.h"

#include "../Character/HeroCharacterBase.h"

ADamageActorProjectileGruxSP::ADamageActorProjectileGruxSP()
{
	ProjectileMovement->InitialSpeed = 0.1f;
	ProjectileMovement->MaxSpeed = 0.1f;

	InitialLifeSpan = 10.0f;
}

void ADamageActorProjectileGruxSP::BeginPlay()
{
	Super::BeginPlay();

	// ProjectileMovement->StopMovementImmediately();
	
	P = Cast<UParticleSystemComponent>(GetComponentByClass(UParticleSystemComponent::StaticClass()));
}

void ADamageActorProjectileGruxSP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HeroGrux && IsValid(HeroGrux))
	{
		auto HeroLocation = HeroGrux->GetActorLocation();

		auto Hero = Cast<AHeroCharacterBase>(HeroGrux);
		if (Hero)
		{
			HeroLocation.X = Hero->ShadowLocation.X;
			HeroLocation.Y = Hero->ShadowLocation.Y;
		}
		
		auto HeroForward = InitForward;

		FRotator ThisRot = FRotator(0.0f, SumTime * 150.0f, 0.0f);
		HeroForward = ThisRot.RotateVector(HeroForward);

		SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * (80 + 120 * SumTime));

		if (P)
		{
			float S = 1.0f + std::min(2.0f, SumTime / 2.0f);
			P->SetWorldScale3D(FVector(S, S, S));
			CollisionComp->SetWorldScale3D(FVector(S, S, S));
		}
	}
}

float ADamageActorProjectileGruxSP::Damage(AActor* Who, float DeltaTime)
{
	if (Who == Owner || Histroy.Contains(Owner)) return 0.0f;

	return Super::Damage(Who, DeltaTime) + 500.0f * DamageScale * DeltaTime;
}