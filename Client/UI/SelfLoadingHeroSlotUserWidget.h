// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "../Instance/MainGameInstance.h"

#include "SelfLoadingHeroSlotUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API USelfLoadingHeroSlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* CenterImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UserNameTextBlock;

public:
	void SetHeroAndUsername(int HeroID, const FString& UserName);
};
