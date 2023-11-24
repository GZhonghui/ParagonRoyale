// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfSkillUserWidget.h"

bool USelfSkillUserWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	return true;
}

void USelfSkillUserWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	RemaingTime -= DeltaTime;
	if (RemaingTime < 0.0f) RemaingTime = -1.0f;

	if (CoolingBar)
	{
		auto ScaledTime = CooldownTime * CooldownScale;
		RemaingTime = std::min(RemaingTime, ScaledTime);
		float P = FMath::Clamp(RemaingTime, 0.0f, ScaledTime) / ScaledTime;
		CoolingBar->SetPercent(P);
	}

	if (CoolTimeText)
	{
		if (RemaingTime > 1.0f)
		{
			int R = FMath::FloorToInt(RemaingTime);
			CoolTimeText->SetText(FText::FromString(FString::FromInt(R)));

			CoolTimeText->SetVisibility(ESlateVisibility::Visible);
		}
		else if (RemaingTime > 0.1f)
		{
			FString R = FString::Printf(TEXT("%.1f"), RemaingTime);
			CoolTimeText->SetText(FText::FromString(R));

			CoolTimeText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CoolTimeText->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (Outline)
	{
		// Outline->SetVisibility((RemaingTime < 0.0f) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		Outline->SetOpacity(Ready() ? 1.0f : 0.7f);
	}
}

void USelfSkillUserWidget::SetSkillID(int ID)
{
	if (GI && GI->SkillInfos.Contains(ID))
	{
		SaveID = ID;
		CooldownTime = GI->SkillInfos[SaveID].CooldownTime;
		SetCenterImageFromPath(GI->SkillInfos[SaveID].IconPath);

		RemaingTime = -1.0f;
	}
}

void USelfSkillUserWidget::SetText(const FString& Text)
{
	if (KeyText)
	{
		KeyText->SetText(FText::FromString(Text));
	}
}

void USelfSkillUserWidget::SetCenterImageFromPath(const FString& ImagePath)
{
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImagePath)));

	if (CenterImage) CenterImage->SetBrushFromTexture(Texture);
}

void USelfSkillUserWidget::Active()
{
	if (!Ready()) return;

	RemaingTime = CooldownTime * CooldownScale;
}

bool USelfSkillUserWidget::Ready()
{
	return RemaingTime < 0.0f;
}