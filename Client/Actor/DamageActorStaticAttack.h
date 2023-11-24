// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorStatic.h"
#include "DamageActorStaticAttack.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorStaticAttack : public ADamageActorStatic
{
	GENERATED_BODY()

public:
	ADamageActorStaticAttack();

public:
	virtual float Damage(AActor* Who, float DeltaTime);
};
