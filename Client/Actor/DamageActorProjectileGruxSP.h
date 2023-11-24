// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorProjectile.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "DamageActorProjectileGruxSP.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorProjectileGruxSP : public ADamageActorProjectile
{
	GENERATED_BODY()

public:
	ADamageActorProjectileGruxSP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual float Damage(AActor* Who, float DeltaTime) override;

public:
	AActor* HeroGrux = nullptr;
	FVector InitForward;

protected:
	UParticleSystemComponent* P = nullptr;
};
