// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Throbber.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"

#include "MediaPlayer.h"
#include "MediaSource.h"

#include "SelfButtonUserWidget.h"
#include "SelfSelectableUserWidget.h"
#include "SelfLoadingHeroSlotUserWidget.h"

#include "../Actor/HeroPreviewActor.h"
#include "../Instance/MainGameInstance.h"

#include "MainUserWidget.generated.h"

/*

Order:
1. Aurora
2. FengMao
3. Grux
4. Sparrow

*/

UCLASS()
class PARAGONROYAL_API UMainUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* UserNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* SkillButton;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* MatchButton;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* QuitButton;

	UPROPERTY(meta = (BindWidget))
	UThrobber* MatchingThrobber;

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HeroName;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* HeroPick_01;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* HeroPick_02;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* HeroPick_03;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* HeroPick_04;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skin_01;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skin_02;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skin_03;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skin_04;

protected:
	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skill_LM;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skill_RM;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skill_Q;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skill_E;

	UPROPERTY(meta = (BindWidget))
	USelfSelectableUserWidget* Skill_Space;

// About Panel
protected:
	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* CloseSkillButton;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillVideo;

	UMediaPlayer* SkillPlayer = nullptr;

// Matching
protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MatchFoundCanvas;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaitingToEnterGameTimer;

// Loading Panel
protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* LoadingPanel;

	UPROPERTY(meta = (BindWidget))
	USelfLoadingHeroSlotUserWidget* LoadingHero_01;

	UPROPERTY(meta = (BindWidget))
	USelfLoadingHeroSlotUserWidget* LoadingHero_02;

	UPROPERTY(meta = (BindWidget))
	USelfLoadingHeroSlotUserWidget* LoadingHero_03;

	UPROPERTY(meta = (BindWidget))
	USelfLoadingHeroSlotUserWidget* LoadingHero_04;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextMapName;

protected:
	AActor* PreviewActor = nullptr;

	int NowHero = 0, NowSkin = 0, NowSkill = 0;

	UPROPERTY(EditDefaultsOnly)
	TArray< TSubclassOf<AHeroPreviewActor> > PreviewActorAuroraClasses;

	UPROPERTY(EditDefaultsOnly)
	TArray< TSubclassOf<AHeroPreviewActor> > PreviewActorFengMaoClasses;

	UPROPERTY(EditDefaultsOnly)
	TArray< TSubclassOf<AHeroPreviewActor> > PreviewActorGruxClasses;

	UPROPERTY(EditDefaultsOnly)
	TArray< TSubclassOf<AHeroPreviewActor> > PreviewActorSparrowClasses;

public:
	void HeroChange(int Which);
	void SkinChange(int Which);
	void SkillChange(int Which);

protected:
	void SetSkinNumber(int Number);
	void SpawnByHeroAndSkin(int Hero, int Skin);
	void PlayVideoByHeroAndSkill(int Hero, int Skill);

public:
	void PushSkillButton();
	void PushMatchButton();
	void PushQuitButton();
	void PushCloseSkillButton();

protected:
	bool IsMatching = false;

// Message From Server
public:
	void MatchFound();

private:
	float WaitingTimeEnterGame = -999.0f;
};
