// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageActorStaticAuroraQ.h"

ADamageActorStaticAuroraQ::ADamageActorStaticAuroraQ()
{
	InitialLifeSpan = 5.2f;
}

void ADamageActorStaticAuroraQ::BeginPlay()
{
	Super::BeginPlay();

	Dragon = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
}

void ADamageActorStaticAuroraQ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SumTime > 5.2f)
	{
		Destroy();
		return;
	}

	if (Dragon)
	{
		const float UpHeight = 260.0f;
		const float UpTime = 1.4f;
		const float ForwardSpeed = 1000.0f;

		if (SumTime < UpTime)
		{
			Dragon->AddLocalOffset(FVector(0.0f, 0.0f, DeltaTime * UpHeight / UpTime));
		}
		if(SumTime > 1.0f)
		{
			Dragon->AddLocalOffset(FVector(0.0f, DeltaTime * ForwardSpeed, 0.0f));

			LastDistanceNext -= DeltaTime * ForwardSpeed;
		}
		if (SumTime > 4.0f)
		{
			Dragon->AddLocalOffset(FVector(0.0f, 0.0f, DeltaTime * 600.0f));
		}

		const float Step = 180.0f;
		if (LastDistanceNext < 0.0f)
		{
			if (IceBlocks.Num())
			{
				auto DragonLocation = Dragon->GetComponentLocation();
				auto ThisLocation = DragonLocation;
				ThisLocation.Z = 0.1f;

				FTransform IceTransform;

				ThisLocation.X += FMath::RandRange(-80.0f, 80.0f);
				ThisLocation.Y += FMath::RandRange(-80.0f, 80.0f);
				IceTransform.SetLocation(ThisLocation);

				IceTransform.SetRotation(FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f).Quaternion());

				auto S = FMath::RandRange(1.6f, 2.4f);
				IceTransform.SetScale3D(FVector(S, S, S));
				
				int Index = (int)FMath::RandRange(0.1f, IceBlocks.Num() - 0.1f);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IceBlocks[Index], IceTransform, true);

				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				if (StaticIceClass)
				{
					auto A = GetWorld()->SpawnActor<ADamageActorStaticAuroraQIce>(StaticIceClass, IceTransform.GetLocation(), IceTransform.GetRotation().Rotator(), ActorSpawnParams);
					if (A)
					{
						A->PlayerID = PlayerID;
						A->Owner = Owner;
						A->DamageScale = DamageScale;
					}
				}
			}

			LastDistanceNext = Step;
		}
	}
}

float ADamageActorStaticAuroraQ::Damage(AActor* Who, float DeltaTime)
{
	return Super::Damage(Who, DeltaTime);
}