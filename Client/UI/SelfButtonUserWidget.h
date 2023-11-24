// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "SelfButtonUserWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(SelfButtonPressedDelegate)

UCLASS()
class PARAGONROYAL_API USelfButtonUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	float FillTime = 0.12f;
	float SumTime = 0.0f;

	bool CursorInSelf = false;
	bool MouseButtonDown = false;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* BackBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Content;

	UPROPERTY(meta = (BindWidget))
	class UButton* MainButton;

protected:
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

public:
	void SetText(const FString& Str);
	void SetColor(const FLinearColor& Color);

public:
	UFUNCTION()
	void Press();

	UFUNCTION()
	void Release();

public:
	SelfButtonPressedDelegate PressedMe;
};
