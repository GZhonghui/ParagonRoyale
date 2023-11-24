// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "../Instance/MainGameInstance.h"

#include "SelfSkillUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API USelfSkillUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* CoolingBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CoolTimeText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KeyText;

	UPROPERTY(meta = (BindWidget))
	class UImage* CenterImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* Outline;

public:
	void SetSkillID(int ID);

public:
	void SetText(const FString& Text);
	void SetCenterImageFromPath(const FString& ImagePath);
	void Active();
	bool Ready();

protected:
	float CooldownTime = 1.0f;
	float RemaingTime = -1.0f;

public:
	float CooldownScale = 1.0f;

public:
	int SaveID = 0;
};
