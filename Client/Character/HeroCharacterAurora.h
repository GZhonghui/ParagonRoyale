// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroCharacterBase.h"

#include "../Actor/DamageActorStaticAttack.h"
#include "../Actor/DamageActorProjectileAuroraRM.h"
#include "../Actor/DamageActorStaticAuroraQ.h"
#include "../Actor/DamageActorStaticAuroraE.h"
#include "../Actor/DamageActorStaticAuroraSP.h"

#include "HeroCharacterAurora.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API AHeroCharacterAurora : public AHeroCharacterBase
{
	GENERATED_BODY()

public:
	AHeroCharacterAurora();
	
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
	UAnimMontage* AnimLM;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticAttack> StaticLMClass;

// Skill RM
protected:
	ADecalActor* DecalRMArrowFront = nullptr;
	ADecalActor* DecalRMArrowBack = nullptr;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialRMArrowFront;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialRMArrowBack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorProjectileAuroraRM> ProjectileRMClass;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimRM;

// Skill Q
protected:
	ADecalActor* DecalQArrowFront = nullptr;
	ADecalActor* DecalQArrowBack = nullptr;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialQArrowFront;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialQArrowBack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticAuroraQ> StaticQClass;

// Skill E
protected:
	ADecalActor* DecalECenter = nullptr;
	ADecalActor* DecalETarget = nullptr;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialECenter;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialETarget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticAuroraE> StaticEClass;

// Skill SP
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticAuroraSP> StaticSPClass;

public:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimSP;
};
