// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfSelectableUserWidget.h"

bool USelfSelectableUserWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	return true;
}

void USelfSelectableUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	SumTime += InDeltaTime * (CursorInSelf ? 1 : -1);
	SumTime = FMath::Clamp(SumTime, 0.0f, FillTime);

	if (FillBar) FillBar->SetPercent(FMath::Clamp(SumTime, 0.0f, FillTime) / FillTime);
	if (OutLine)
	{
		if (Selected || (CursorInSelf && FillTime - SumTime < 1e-1))
		{
			OutLine->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			OutLine->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void USelfSelectableUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	CursorInSelf = true;
}

void USelfSelectableUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	CursorInSelf = false;
}

FReply USelfSelectableUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto Result = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(FKey(TEXT("LeftMouseButton"))))
	{
		Press();

		return FReply::Handled();
	}

	return Result;
}

void USelfSelectableUserWidget::Press()
{
	SelectedMe.Broadcast(SelfID);
	Selected = true;
}

void USelfSelectableUserWidget::SetImageFromPath(const FString& ImagePath)
{
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImagePath)));

	if(Texture) CenterImage->SetBrushFromTexture(Texture);
}

void USelfSelectableUserWidget::SetColor(const FLinearColor& Color)
{
	CenterImage->SetColorAndOpacity(Color);
}

void USelfSelectableUserWidget::SetText(const FString& ShowText)
{
	auto TextPlace = Cast<UTextBlock>(GetWidgetFromName(FName(TEXT("CenterText"))));
	if (TextPlace)
	{
		TextPlace->SetText(FText::FromString(ShowText));
	}
}