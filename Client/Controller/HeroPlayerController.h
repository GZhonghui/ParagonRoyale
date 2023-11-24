// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Engine/World.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInterface.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "../Character/HeroCharacterBase.h"
#include "../UI/GameUserWidget.h"

#include "HeroPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API AHeroPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

public:
	void OnInputMoveForward(float Value);
	void OnInputMoveRight(float Value);
	void OnInputTab();
	void OnInputSkillLMouse();
	void OnInputSkillRMouse();
	void OnInputSkillQ();
	void OnInputSkillE();
	void OnInputSkillSpace();
	void OnInputMenu();

protected:
	std::unordered_set<int> WantBuyIDs;
	std::queue<int> BuyQueue;

private:
	float LastBuyPassed = 0.0f;

public:
	void OnPawnRecvItem(int ItemID);

public:
	UFUNCTION()
	void OnWantBuySome(int SlotIndex);

public:
	void BuyItem(int ItemPanelIndex);
	bool TryBuyItem(int ItemPanelIndex);

	void AttachItem(int ItemID);

public:
	void ShowWaitingUI();

public:
	void ReleaseSkill(int SkillID);

public:
	void AddGameUIToViewPort();
	bool AddedGameUIToViewPort = false;

protected:
	FVector GetMouseWorldLocation();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameUserWidget> GameUIClass;

	UGameUserWidget* GameUI;

	int SaveHeroID = 0;
	APawn* SaveHero = nullptr;

	bool ShowingTalent = false;
};
