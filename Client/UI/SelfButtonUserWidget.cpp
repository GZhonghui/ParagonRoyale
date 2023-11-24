// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfButtonUserWidget.h"

bool USelfButtonUserWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	if (MainButton)
	{
		MainButton->OnPressed.AddDynamic(this, &USelfButtonUserWidget::Press);
		MainButton->OnReleased.AddDynamic(this, &USelfButtonUserWidget::Release);
	}

	return true;
}

void USelfButtonUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!MouseButtonDown)
	{
		SumTime += InDeltaTime * (CursorInSelf ? 1 : -1);
		SumTime = FMath::Clamp(SumTime, 0.0f, FillTime);
	}

	if(BackBar) BackBar->SetPercent(MouseButtonDown ? 0.0f : FMath::Clamp(SumTime, 0.0f, FillTime) / FillTime);
}

void USelfButtonUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	CursorInSelf = true;
}

void USelfButtonUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	CursorInSelf = false;
}

void USelfButtonUserWidget::SetText(const FString& Str)
{
	if (Content) Content->SetText(FText::FromString(Str));
}

void USelfButtonUserWidget::SetColor(const FLinearColor& Color)
{
	BackBar->SetFillColorAndOpacity(Color);
}

void USelfButtonUserWidget::Press()
{
	MouseButtonDown = true;

	PressedMe.Broadcast();
}

void USelfButtonUserWidget::Release()
{
	MouseButtonDown = false;
}