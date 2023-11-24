// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorProjectile.h"
#include "DamageActorProjectileSparrowRM.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorProjectileSparrowRM : public ADamageActorProjectile
{
	GENERATED_BODY()
	
public:
	ADamageActorProjectileSparrowRM();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual float Damage(AActor* Who, float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ParticleOnDestroy;
};
