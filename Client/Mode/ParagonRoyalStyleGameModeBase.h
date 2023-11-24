// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParagonRoyalGameModeBase.h"

#include "Math/RandomStream.h"

#include "../Instance/MainGameInstance.h"
#include "../Actor/StyleBlockActor.h"
#include "../Actor/StyleGrassActor.h"
#include "../Actor/StylePickableActor.h"

#include "ParagonRoyalStyleGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONROYAL_API AParagonRoyalStyleGameModeBase : public AParagonRoyalGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	void PlaceAt(int X, int Y, int Type);

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AStyleBlockActor> BlockClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AStyleGrassActor> GrassClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AStylePickableActor> PickableClass;
};
