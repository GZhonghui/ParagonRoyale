// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorProjectile.h"

#include "DamageActorProjectileAuroraRM.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorProjectileAuroraRM : public ADamageActorProjectile
{
	GENERATED_BODY()

public:
	ADamageActorProjectileAuroraRM();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual float Damage(AActor* Who, float DeltaTime);

public:
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ParticleOnDestroy;
};
