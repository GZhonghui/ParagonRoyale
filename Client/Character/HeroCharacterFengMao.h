// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroCharacterBase.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "../Actor/DamageActorStaticAttack.h"
#include "../Actor/DamageActorStaticFengMaoRM.h"
#include "../Actor/DamageActorStaticFengMaoQ.h"
#include "../Actor/DamageActorStaticFengMaoE.h"

#include "HeroCharacterFengMao.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API AHeroCharacterFengMao : public AHeroCharacterBase
{
	GENERATED_BODY()

public:
	AHeroCharacterFengMao();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
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
public:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimRM;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticFengMaoRM> StaticRMClass;

// Skill Q
protected:
	ADecalActor* DecalQTarget = nullptr;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialQTarget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticFengMaoQ> StaticQClass;

// Skill E
protected:
	ADecalActor* DecalEArrowFront = nullptr;
	ADecalActor* DecalEArrowBack = nullptr;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialEArrowFront;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterialEArrowBack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorStaticFengMaoE> StaticEClass;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AnimE;

// Skill SP
public:
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ParticleSPSmoke;
};
