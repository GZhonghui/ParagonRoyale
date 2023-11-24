// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStatic.h"

ADamageActorStatic::ADamageActorStatic()
{

}

void ADamageActorStatic::BeginPlay()
{
    Super::BeginPlay();
}

void ADamageActorStatic::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float ADamageActorStatic::Damage(AActor* Who, float DeltaTime)
{
    return Super::Damage(Who, DeltaTime);
}