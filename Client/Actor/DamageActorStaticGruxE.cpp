// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticGruxE.h"

ADamageActorStaticGruxE::ADamageActorStaticGruxE()
{

}

void ADamageActorStaticGruxE::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageActorStaticGruxE::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Spawned && SumTime > 3.2f)
	{
		auto World = GetWorld();
		if (World && ProjectileClass)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FRotator Rot(0, 15, 0);
			auto Forward = GetActorForwardVector();
			auto ActorLocation = GetActorLocation();

			for (int i = 0; i < 24; i += 1)
			{
				Forward = Rot.RotateVector(Forward);

				auto SpawnProjectileLocation = Forward * 80 + GetActorLocation();

				FRotator ProjectileRot = UKismetMathLibrary::FindLookAtRotation(ActorLocation, SpawnProjectileLocation);

				auto A = World->SpawnActor<ADamageActorProjectileGruxE>(ProjectileClass, SpawnProjectileLocation, ProjectileRot, ActorSpawnParams);
				if (A)
				{
					A->PlayerID = PlayerID;
					A->Owner = Owner;
					A->DamageScale = DamageScale;
				}
			}
		}
		Spawned = true;
	}
	if (SumTime > 5.4f)
	{
		Destroy();
	}
}

float ADamageActorStaticGruxE::Damage(AActor* Who, float DeltaTime)
{
	return Super::Damage(Who, DeltaTime);
}