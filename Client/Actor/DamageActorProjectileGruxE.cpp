// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorProjectileGruxE.h"

ADamageActorProjectileGruxE::ADamageActorProjectileGruxE()
{

}

void ADamageActorProjectileGruxE::BeginPlay()
{
    Super::BeginPlay();
}

void ADamageActorProjectileGruxE::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float ADamageActorProjectileGruxE::Damage(AActor* Who, float DeltaTime)
{
    if (Who == Owner || Histroy.Contains(Who)) return 0.0f;

    ShouldDestroy = true;

    if (ParticleOnDestroy)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleOnDestroy, GetActorTransform(), true);
    }

    return Super::Damage(Who, DeltaTime) + 400.0f * DamageScale;
}