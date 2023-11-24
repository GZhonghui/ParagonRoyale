// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorStatic.h"
#include "DamageActorStaticFengMaoRM.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorStaticFengMaoRM : public ADamageActorStatic
{
	GENERATED_BODY()

public:
	ADamageActorStaticFengMaoRM();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual float Damage(AActor* Who, float DeltaTime) override;

public:
	AActor* HeroFengMao = nullptr;
};
