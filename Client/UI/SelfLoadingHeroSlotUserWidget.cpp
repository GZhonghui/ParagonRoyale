// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfLoadingHeroSlotUserWidget.h"

void USelfLoadingHeroSlotUserWidget::SetHeroAndUsername(int HeroID, const FString& UserName)
{
	if (CenterImage && GI)
	{
		const FString& ImagePath = GI->HeroInfos[HeroID].MiniImagePath;
		UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(ImagePath)));

		if (Texture) CenterImage->SetBrushFromTexture(Texture);
	}

	if (UserNameTextBlock)
	{
		UserNameTextBlock->SetText(FText::FromString(UserName));
	}
}
