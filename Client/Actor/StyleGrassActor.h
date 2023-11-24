// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "../Instance/MainGameInstance.h"

#include "StyleGrassActor.generated.h"

UCLASS()
class PARAGONROYAL_API AStyleGrassActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStyleGrassActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	int X = 0;
	int Y = 0;

private:
	UMaterialInstanceDynamic* MiGrass;
};
