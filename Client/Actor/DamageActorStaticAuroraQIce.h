// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorStatic.h"
#include "DamageActorStaticAuroraQIce.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorStaticAuroraQIce : public ADamageActorStatic
{
	GENERATED_BODY()
public:
	ADamageActorStaticAuroraQIce();

public:
	virtual float Damage(AActor* Who, float DeltaTime) override;
};
