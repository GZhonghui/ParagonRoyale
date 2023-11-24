// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroCharacterBase.h"

#include "Particles/ParticleSystem.h"

#include "../Actor/DamageActorStaticAttack.h"
#include "../Actor/DamageActorProjectileGruxRM.h"
#include "../Actor/DamageActorStaticGruxQ.h"
#include "../Actor/DamageActorStaticGruxE.h"
#include "../Actor/DamageActorProjectileGruxSP.h"

#include "HeroCharacterGrux.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API AHeroCharacterGrux : public AHeroCharacterBase
{
	GENERATED_BODY()

public:
	AHeroCharacterGrux();
	
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

public:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimLM;

// Skill LM
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticAttack> StaticLMClass;

// Skill RM
protected:
	ADecalActor* DecalRMArrowFront = nullptr;
	ADecalActor* DecalRMArrowBack = nullptr;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialRMArrowFrontBugFix;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialRMArrowBackBugFix;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimRM;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorProjectileGruxRM> ProjectileRMClass;

// Skill Q
protected:
	ADecalActor* DecalQ = nullptr;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialQBugFix;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticGruxQ> StaticQClass;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimQ;

// Skill E
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticGruxE> StaticEClass;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimE;

// Skill SP
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorProjectileGruxSP> ProjectileSPClass;
};
