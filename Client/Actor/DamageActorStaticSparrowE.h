// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorStatic.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"

#include "DamageActorStaticSparrowE.generated.h"

UCLASS()
class PARAGONROYAL_API ADamageActorStaticSparrowE : public ADamageActorStatic
{
	GENERATED_BODY()

public:
	ADamageActorStaticSparrowE();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual float Damage(AActor* Who, float DeltaTime) override;

protected:
	TArray<UParticleSystemComponent*> P;
	UCapsuleComponent* Collision;
};
