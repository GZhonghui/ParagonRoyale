// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfShopItemDetailUserWidget.h"

void USelfShopItemDetailUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	FlashPosition();
}

void USelfShopItemDetailUserWidget::FlashPosition()
{
	static APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		float X, Y;
		bool R = PC->GetMousePosition(X, Y);
		if (R)
		{
			SetPositionInViewport(FVector2D(X + 16, Y - 64));
		}
	}
}

void USelfShopItemDetailUserWidget::SetItemID(int ID)
{
	if (GI)
	{
		const TMap<int, FItemInfo>& LocalItemInfos = GI->ItemInfos;
		if (LocalItemInfos.Contains(ID))
		{
			SaveID = ID;

			ItemName->SetText(FText::FromString(LocalItemInfos[SaveID].Name));
			ItemDescription->SetText(FText::FromString(LocalItemInfos[SaveID].Description));
			ItemTips->SetText(FText::FromString(LocalItemInfos[SaveID].Tip));
			ItemPrice->SetText(FText::FromString(FString::Printf(TEXT("Price : %d"), LocalItemInfos[SaveID].Cost)));
		}
	}
}