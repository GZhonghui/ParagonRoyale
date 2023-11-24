// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroAnimInstanceBase.h"

void UHeroAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto SelfPawn = Cast<AHeroCharacterBase>(TryGetPawnOwner());

	if (SelfPawn)
	{
		// Speed = SelfPawn->GetVelocity().Size();
		Speed = SelfPawn->MoveSpeed;

		auto LookatTarget = SelfPawn->LookatTarget;
		auto ThisLocation = SelfPawn->GetActorLocation();
		FVector Target = FVector(LookatTarget.X, LookatTarget.Y, ThisLocation.Z);

		// auto Rot = SelfPawn->GetActorRotation(); // Why this doesn't work?
		auto Rot = UKismetMathLibrary::FindLookAtRotation(ThisLocation, Target);

		Direction = CalculateDirection(SelfPawn->GetVelocity(), Rot);
	}
	else
	{
		Speed = 0.0f;
		Direction = 0.0f;
	}

	IsRunning = Speed > 1e-1;
}