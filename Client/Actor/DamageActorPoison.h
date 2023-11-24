// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorBase.h"

#include "../Instance/MainGameInstance.h"

#include "DamageActorPoison.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorPoison : public ADamageActorBase
{
	GENERATED_BODY()

public:
	ADamageActorPoison();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual float Damage(AActor* Who, float DeltaTime) override;

private:
	const float MoveSpeed = 25.0f;
	const float DamagePerSecond = 60.0f;

	FVector InitLocation;
};
