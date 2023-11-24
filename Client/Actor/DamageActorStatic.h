// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorBase.h"

#include "DamageActorStatic.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorStatic : public ADamageActorBase
{
	GENERATED_BODY()

public:
	ADamageActorStatic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual float Damage(AActor* Who, float DeltaTime) override;
};
