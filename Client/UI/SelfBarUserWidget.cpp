// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfBarUserWidget.h"

bool USelfBarUserWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	// In Blueprint

	return true;
}

void USelfBarUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TargetValue > FrontValue)
	{
		FrontValue += FMath::Min(TargetValue - FrontValue, InDeltaTime * FrontSpeed);
	}
	else
	{
		FrontValue -= FMath::Min(FrontValue - TargetValue, InDeltaTime * FrontSpeed);
	}
	
	if (FrontValue > BackValue)
	{
		BackValue = FrontValue;
	}
	else
	{
		BackValue -= FMath::Min(BackValue - FrontValue, InDeltaTime * BackSpeed);
	}

	FrontBar->SetPercent(FrontValue / MaxValue);
	BackBar->SetPercent(BackValue / MaxValue);

	UCanvasPanelSlot* GridAsPanelSlot = Cast<UCanvasPanelSlot>(ImageBock->Slot);
	if (GridAsPanelSlot)
	{
		float Left = 0.0f, Right = 750.0f - 12.0f; // HARD CODE
		GridAsPanelSlot->SetPosition(FVector2D((Right - Left) * FrontValue / MaxValue, 0));
	}
}

void USelfBarUserWidget::SetMax(float MaxVal)
{
	MaxValue = MaxVal;
}

void USelfBarUserWidget::SetNow(float NowVal)
{
	TargetValue = FMath::Clamp(NowVal, 0.0f, MaxValue);
}

void USelfBarUserWidget::Change(float Delta)
{
	TargetValue = FMath::Clamp(TargetValue + Delta, 0.0f, MaxValue);
}

float USelfBarUserWidget::Last() const
{
	return TargetValue;
}

float USelfBarUserWidget::Limit() const
{
	return MaxValue;
}

void USelfBarUserWidget::ForceFlash()
{
	FrontValue = TargetValue;
	BackValue = TargetValue;
}