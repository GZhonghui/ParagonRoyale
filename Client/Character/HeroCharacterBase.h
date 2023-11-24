// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInterface.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/DecalActor.h"

#include "../Actor/ShadowActor.h"
#include "../Actor/DamageActorBase.h"
#include "../Instance/MainGameInstance.h"
#include "../Component/HeadBarActorComponent.h"

#include "HeroCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(ReleaseSkill, int)
DECLARE_MULTICAST_DELEGATE_OneParam(RecvItem, int)
DECLARE_MULTICAST_DELEGATE_OneParam(MeDie, FString)

UCLASS()
class PARAGONROYAL_API AHeroCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHeroCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	class USpringArmComponent* SpringArm;
	class UCameraComponent* Camera;

protected:
	UPROPERTY(EditDefaultsOnly)
	UHeadBarActorComponent* HeadBarComponent = nullptr;

public:
	FString UserName;

// Hero Infos
protected:
	float HealthNow = 100.0f;
	float HealthMax = 100.0f;
	float HealthRecover = 10.0f;

	float ManaNow = 100.0f;
	float ManaMax = 100.0f;
	float ManaRecover = 10.0f;

	float MoneyNow = 0;
	float MoneyAddSpeed = 3.2f;

	float DamageScale = 1.0f;

	int SkillsID[6];

	void GetInfoFromInstance(int InputHeroID);

// Move States
public:
	FVector2D LookatTarget;
	FVector2D LookatTargetLimited; // Use while not allow turn

	FVector2D NetSimulatedMoveStartPoint;
	FVector2D NetSimulatedMoveEndPoint;

	FVector2D NetSimulatedTurnStartPoint;
	FVector2D NetSimulatedTurnEndPoint;

	PClientStep_Action WillDoAction; // Will do at the end of the step

	float MoveSpeed = 0.0f;

public:
	FVector2D ShadowLocation;
	AShadowActor* Shadow;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AShadowActor> ShadowClass;

// Other States
public:
	bool AllowMove = true;
	bool AllowTurn = true;
	bool AllowSkill = true;
	bool AllowDamage = true;
	bool AllowView = true;

	float SingRemain = 0.0f;
	float SingTotal = 0.0f;

	float AwayFromDamageNeedTime = 5.0f;
	float LastDamagePassed = 5.0f;

	bool IsDead = false;
	bool ShouldDestroy = false;
	bool DoDestroyed = false;
	bool Won = false;

protected:
	void StartSing(float Time, bool AllowMoveInThis, bool AllowTurnInThis);
	void TickSing(float DeltaTime);

protected:
	TSet<ADamageActorBase*> DamageActors;

protected:
	void TickDamge(float DeltaTime);

// To Update State
public:
	void CauseDamage(int FromWho, float Damge);
	bool SpendMana(float ManaValue);
	bool SpendMoney(float MoneyValue);

	void AttachItem(int ItemID); // Also in Player Controller

// Skills
public:
	virtual bool SkillLM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet);
	virtual bool SkillRM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet);
	virtual bool SkillQ(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet);
	virtual bool SkillE(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet);
	virtual bool SkillSP(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet);

	virtual void CancelAllTargeting();

protected:
	int TargetingSkill = 0;

// Tools
public:
	FVector GetMouseWorldLocation();
	bool IsLocalPlayer() const;
	void SetLocationOnPlane(float X, float Y);
	void LookatOnPlane(float X, float Y);
	void InWhichBlockNow(int& X, int& Y);

public:
	int HeroID = 0; // Which Hero
	int PlayerID = 0; // Player Index in a Single Game, [1,4]

// Misc
public:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimDie;

public:
	MeDie OnMeDie;

public:
	UFUNCTION()
	void OnSetHeadbarShow(bool Show);

public:
	UFUNCTION()
	void OnNetMove(int NetPlayerID, float X, float Y);

	UFUNCTION()
	void OnNetTurn(int NetPlayerID, float X, float Y);

public:
	UFUNCTION()
	void OnStepStart(int NowStep);

	UFUNCTION()
	void OnHaveStepMessage(int NetPlayerID, int IsEmpty, FClientStep Step);

private:
	float OneStepTimer = 0.0f;
	PClientStep ClientStep; // Will Send
	std::queue<PClientStep> RecvedSteps;
	PClientStep RecvedLastStep; // Without Action

	int ContinuousEmptyStep = 0;

// Sync Release Skill
public:
	float LastSendSkillPassed = 0.0f;
	ReleaseSkill OnReleaseSkill;

// Sync Item
public:
	RecvItem OnRecvItem;
	std::unordered_set<int> AlreadyHaveItem;

public:
	UFUNCTION()
	void OnCursorEnter(class UPrimitiveComponent* OtherComp);
	UFUNCTION()
	void OnCursorLeave(class UPrimitiveComponent* OtherComp);

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	friend class AHeroPlayerController;
};
