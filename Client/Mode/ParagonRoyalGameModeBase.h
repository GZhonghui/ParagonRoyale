// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "../Instance/MainGameInstance.h"
#include "../Character/HeroCharacterBase.h"
#include "../Character/HeroCharacterAurora.h"
#include "../Character/HeroCharacterFengMao.h"
#include "../Character/HeroCharacterGrux.h"
#include "../Character/HeroCharacterSparrow.h"

#include "ParagonRoyalGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class PARAGONROYAL_API AParagonRoyalGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	AHeroCharacterBase* SpawnHeroAt(int HeroID, int SkinID, float X, float Y);

public:
	UFUNCTION(BlueprintCallable)
	void IntroDoneMapReady();

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AHeroCharacterAurora>> AuroraBpWithSkins;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AHeroCharacterFengMao>> FengMaoBpWithSkins;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AHeroCharacterGrux>> GruxBpWithSkins;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AHeroCharacterSparrow>> SparrowBpWithSkins;
};
