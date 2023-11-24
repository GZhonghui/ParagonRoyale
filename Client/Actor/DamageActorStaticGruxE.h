// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorStatic.h"

#include "Kismet/KismetMathLibrary.h"

#include "DamageActorProjectileGruxE.h"

#include "DamageActorStaticGruxE.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API ADamageActorStaticGruxE : public ADamageActorStatic
{
	GENERATED_BODY()

public:
	ADamageActorStaticGruxE();

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
	TSubclassOf<ADamageActorProjectileGruxE> ProjectileClass;

private:
	bool Spawned = false;
};
