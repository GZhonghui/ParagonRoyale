// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Kismet/GameplayStatics.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "DamageActorBase.generated.h"

UCLASS()
class PARAGONROYAL_API ADamageActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual float Damage(AActor* Who, float DeltaTime);

protected:
	TSet<AActor*> Histroy;

protected:
	float SumTime = 0.0f;

	bool ShouldDestroy = false;

public:
	int PlayerID = 0;
	AActor* Owner = nullptr;

	float DamageScale = 1.0f;
};
