// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "../Instance/MainGameInstance.h"

#include "SelfShopItemDetailUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API USelfShopItemDetailUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemTips;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemPrice;

public:
	void FlashPosition();
	void SetItemID(int ID);

protected:
	int SaveID = 0;
};
