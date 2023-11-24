// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "SelfHeadBarUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API USelfHeadBarUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* Health;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameTextBlock;
};
