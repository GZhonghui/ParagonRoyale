// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroCharacterBase.h"

#include "../Actor/DamageActorProjectileSparrowLM.h"
#include "../Actor/DamageActorProjectileSparrowRM.h"
#include "../Actor/DamageActorStaticSparrowQ.h"
#include "../Actor/DamageActorStaticSparrowE.h"
#include "../Actor/DamageActorStaticSparrowSP.h"

#include "HeroCharacterSparrow.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API AHeroCharacterSparrow : public AHeroCharacterBase
{
	GENERATED_BODY()

public:
	AHeroCharacterSparrow();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual bool SkillLM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet) override;
	virtual bool SkillRM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet) override;
	virtual bool SkillQ(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet) override;
	virtual bool SkillE(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet) override;
	virtual bool SkillSP(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet) override;

	virtual void CancelAllTargeting();

// Skill LM
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorProjectileSparrowLM> ProjectileLM;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimLM;

// Skill RM
protected:
	ADecalActor* DecalRMArrowFront[5] = { nullptr };
	ADecalActor* DecalRMArrowBack[5] = { nullptr };

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialRMArrowFront;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialRMArrowBack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorProjectileSparrowRM> ProjectileRM;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimRM;

// Skill Q
protected:
	ADecalActor* DecalQ[5] = { nullptr };

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialQ;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticSparrowQ> StaticQ;

// Skill E
protected:
	ADecalActor* DecalECenter = nullptr;
	ADecalActor* DecalETarget = nullptr;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialECenterBugFix;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialETargetBugFix;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticSparrowE> StaticEClassBugFix;

// Skill SP
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticSparrowSP> StaticSP;
};
