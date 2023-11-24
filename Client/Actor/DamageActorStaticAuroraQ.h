// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorStatic.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DamageActorStaticAuroraQIce.h"

#include "DamageActorStaticAuroraQ.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorStaticAuroraQ : public ADamageActorStatic
{
	GENERATED_BODY()

public:
	ADamageActorStaticAuroraQ();

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
	TArray<UParticleSystem*> IceBlocks;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticAuroraQIce> StaticIceClass;

protected:
	USkeletalMeshComponent* Dragon = nullptr;

	float LastDistanceNext = 480.0f;
};
