// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include <unordered_set>

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/CanvasPanel.h"

#include "SelfButtonUserWidget.h"
#include "SelfSkillUserWidget.h"
#include "SelfBarUserWidget.h"
#include "SelfShopItemUserWidget.h"

#include "GameUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API UGameUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetSkills(const int* Skills);
	
public:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainPanel;

	// Skills
	UPROPERTY(meta = (BindWidget))
	USelfSkillUserWidget* Skill_Tab;

	UPROPERTY(meta = (BindWidget))
	USelfSkillUserWidget* Skill_LM;

	UPROPERTY(meta = (BindWidget))
	USelfSkillUserWidget* Skill_RM;

	UPROPERTY(meta = (BindWidget))
	USelfSkillUserWidget* Skill_Q;

	UPROPERTY(meta = (BindWidget))
	USelfSkillUserWidget* Skill_E;

	UPROPERTY(meta = (BindWidget))
	USelfSkillUserWidget* Skill_Space;

	// Health & Mana State
	UPROPERTY(meta = (BindWidget))
	USelfBarUserWidget* HealthBar;

	UPROPERTY(meta = (BindWidget))
	USelfBarUserWidget* ManaBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ManaText;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* WaitingPlayerPanel;

	// Items
	UPROPERTY(meta = (BindWidget))
	USelfShopItemUserWidget* Item_01;

	UPROPERTY(meta = (BindWidget))
	USelfShopItemUserWidget* Item_02;

	UPROPERTY(meta = (BindWidget))
	USelfShopItemUserWidget* Item_03;

	UPROPERTY(meta = (BindWidget))
	USelfShopItemUserWidget* Item_04;

	UPROPERTY(meta = (BindWidget))
	USelfShopItemUserWidget* Item_05;

	UPROPERTY(meta = (BindWidget))
	USelfShopItemUserWidget* Item_06;

	UPROPERTY(meta = (BindWidget))
	USelfShopItemUserWidget* Item_07;

	UPROPERTY(meta = (BindWidget))
	USelfShopItemUserWidget* Item_08;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyText;

	// Skill Sing
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* SingCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SingProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock*	SingTextBlock;

	// Menu
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MenuPanel;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* MenuResume;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* MenuQuit;

	// Game Result
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* GameResultPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextResultYouDie;

	UPROPERTY(meta = (BindWidget))
	UImage* ImageResultChicken;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextResultRank;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* VoteText;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* ButtonVoteYes;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* ButtonVoteNo;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* ButtonResultQuit;

	// Info
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoGameTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoStepCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoDelay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoCheck;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoLost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoLastPlayerNumber;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* BackPanel;

	// Player Die
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextPlayerDied;

public:
	int LastMoney = 0;

public:
	void UpdateHealth(float Now, float Max, float Recover);
	void UpdateMana(float Now, float Max, float Recover);
	void UpdateMoney(int Money);
	void UpdateSingState(float Last, float Total);
	void UpdateInfo();

public:
	void OnPushResume();
	void OnPushQuit();
	void OnPushVote();

public:
	void OpenTalent(bool State);
	void OpenMenu();

public:
	void ShowPlayerDie(FString UserName);

private:
	bool WillQuitGame = false;

private:
	float DiePanelTime = 100.0f;
	bool ShowingDiePanel = false;

public:
	void GameRunning(bool Running); // Game Running or Waiting
};
