// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroPreviewAnimInstance.h"

void UHeroPreviewAnimInstance::IdleEnd()
{
	BackToIdle = false;
	WhichNext = (int)FMath::FRandRange(1.1f, 4.9f); // HARD CODE
}

void UHeroPreviewAnimInstance::EmoteEnd()
{
	WhichNext = 0;
	BackToIdle = true;
}