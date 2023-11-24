// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Materials/MaterialInterface.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"

#include "SelfBarUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API USelfBarUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* FrontMaterial;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UProgressBar* FrontBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* BackBar;

	UPROPERTY(meta = (BindWidget))
	class UImage* ImageBock;

public:
	void SetMax(float MaxVal);
	void SetNow(float NowVal);
	void Change(float Delta);

	float Last() const;
	float Limit() const;

	void ForceFlash();

protected:
	float MaxValue = 1000.0f;
	float TargetValue = 1000.0f;
	float FrontValue = 1000.0f;
	float BackValue = 1000.0f;

	const float FrontSpeed = 1200.0f;
	const float BackSpeed = 400.0f;
};
