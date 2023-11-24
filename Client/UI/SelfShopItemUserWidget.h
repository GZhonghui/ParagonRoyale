// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "SelfShopItemDetailUserWidget.h"

#include "../Instance/MainGameInstance.h"

#include "SelfShopItemUserWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(SelfItemBuyDelegate, int)

UCLASS()
class PARAGONROYAL_API USelfShopItemUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* FillBar;

	UPROPERTY(meta = (BindWidget))
	class UImage* OutLine;

	UPROPERTY(meta = (BindWidget))
	class UImage* CenterImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBuyOrder;

protected:
	float FillTime = 0.12f;
	float SumTime = 0.0f;

	bool CursorInSelf = false;

	bool Bought = false;

	int CostMoney = 0;

public:
	void CheckGenerateDetailPanel();

protected:
	SelfItemBuyDelegate OnBuyMe;
	SelfItemBuyDelegate OnWantBuyMe;

public:
	void Buy(bool TryBuy);

	void SetItemID(int ID);

protected:
	// Different!
	int SaveID = 0; // For Item
	int SelfID = 0; // For Index

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<USelfShopItemDetailUserWidget> DetailPanelClass;

	USelfShopItemDetailUserWidget* DetailPanel = nullptr;

public:
	friend class AHeroPlayerController;
	friend class UGameUserWidget;
};
