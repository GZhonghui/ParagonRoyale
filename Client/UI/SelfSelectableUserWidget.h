// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "../Instance/MainGameInstance.h"

#include "SelfSelectableUserWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(SelfSelectedDelegate, int)

UCLASS()
class PARAGONROYAL_API USelfSelectableUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	float FillTime = 0.12f;
	float SumTime = 0.0f;

	bool CursorInSelf = false;
	
public:
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	void Press();

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* FillBar;

	UPROPERTY(meta = (BindWidget))
	class UImage* OutLine;

	UPROPERTY(meta = (BindWidget))
	class UImage* CenterImage;

public:
	void SetImageFromPath(const FString& ImagePath);
	void SetColor(const FLinearColor& Color);
	void SetText(const FString& ShowText);

public:
	bool Selected = false;
	int SelfID = 0;

public:
	SelfSelectedDelegate SelectedMe;
};
