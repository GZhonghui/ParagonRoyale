// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfShopItemUserWidget.h"

bool USelfShopItemUserWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	return true;
}

void USelfShopItemUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	SumTime += InDeltaTime * (CursorInSelf ? 1 : -1);
	SumTime = FMath::Clamp(SumTime, 0.0f, FillTime);

	if (FillBar) FillBar->SetPercent(FMath::Clamp(SumTime, 0.0f, FillTime) / FillTime);
	if (OutLine)
	{
		if (Bought || (CursorInSelf && FillTime - SumTime < 1e-1))
		{
			// OutLine->SetVisibility(ESlateVisibility::Visible);
			OutLine->SetOpacity(1.0f);
		}
		else
		{
			// OutLine->SetVisibility(ESlateVisibility::Hidden);
			OutLine->SetOpacity(0.4f);
		}
	}

	CheckGenerateDetailPanel();
}

void USelfShopItemUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (DetailPanel)
	{
		DetailPanel->FlashPosition();
		DetailPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	CursorInSelf = true;
}

void USelfShopItemUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (DetailPanel)
	{
		DetailPanel->SetVisibility(ESlateVisibility::Hidden);
	}

	CursorInSelf = false;
}

FReply USelfShopItemUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	auto Result = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(FKey(TEXT("RightMouseButton"))))
	{
		Buy(true);

		return FReply::Handled();
	}
	else if (InMouseEvent.IsMouseButtonDown(FKey(TEXT("LeftMouseButton"))))
	{
		if (!Bought)
		{
			OnWantBuyMe.Broadcast(SelfID);
		}
		return FReply::Handled(); // Think...
	}

	return Result;
}

void USelfShopItemUserWidget::CheckGenerateDetailPanel()
{
	if (!DetailPanel)
	{
		// Why Cause Screen Flash
		DetailPanel = CreateWidget<USelfShopItemDetailUserWidget>(GetWorld(), DetailPanelClass);
		DetailPanel->SetVisibility(ESlateVisibility::Hidden);
		DetailPanel->AddToViewport();

		// Can`t Move
		/*
		if (ItemDetailBugFix)
		{
			DetailPanel = ItemDetailBugFix;
		}
		*/

		if (DetailPanel && SaveID)
		{
			DetailPanel->SetItemID(SaveID);
		}
	}
}

void USelfShopItemUserWidget::Buy(bool TryBuy)
{
	if (!Bought)
	{
		if (TryBuy)
		{
			if (SelfID)
			{
				OnBuyMe.Broadcast(SelfID); // Broadcast Index, not Item ID
			}	
		}
		else
		{
			Bought = true;
			OutLine->SetColorAndOpacity(FLinearColor(F(248), F(188), F(49), 1.0f));

			if (DetailPanel)
			{
				DetailPanel->ItemTips->SetText(FText::FromString(TEXT("Bought")));
			}
		}
	}
}

void USelfShopItemUserWidget::SetItemID(int ID)
{
	if (!GI || !GI->ItemInfos.Contains(ID)) return;
	
	SaveID = ID;
	const FItemInfo& Info = GI->ItemInfos[SaveID];

	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(Info.IconPath)));

	if (Texture) CenterImage->SetBrushFromTexture(Texture);

	CostMoney = Info.Cost;

	if (DetailPanel)
	{
		DetailPanel->SetItemID(SaveID);
	}
}