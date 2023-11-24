// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HeroPreviewAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API UHeroPreviewAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool LevelStartFin = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool BackToIdle = false;

	UPROPERTY(BlueprintReadWrite)
	int WhichNext = 0;

	UFUNCTION(BlueprintCallable)
	void IdleEnd();

	UFUNCTION(BlueprintCallable)
	void EmoteEnd();
};
