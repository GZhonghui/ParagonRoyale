// Copyright Epic Games, Inc. All Rights Reserved.


#include "ParagonRoyalGameModeBase.h"

#include "../Controller/HeroPlayerController.h"

void AParagonRoyalGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (!GI) return;

	Print(TEXT("Initing Game"));

	// Set the Location
	float X[5], Y[5];
	float BlockWidth = GI->BlockWidthInMap;

	for (int i = 1; i <= 4; i += 1)
	{
		X[i] = GI->PlayerInitLocationAsBlockX[i] * BlockWidth;
		Y[i] = GI->PlayerInitLocationAsBlockY[i] * BlockWidth;
	}

	GI->AllCharacters.Reset();

	for (int i = 0; i < std::min(4, GI->MatchFoundRespondResult.otherplayers_size()); i += 1)
	{
		const PMatchFoundRespond_POtherPlayerInfo& HeroInfo = GI->MatchFoundRespondResult.otherplayers(i);
		auto Hero = SpawnHeroAt(HeroInfo.heroid(), HeroInfo.skinid(), X[HeroInfo.playerid()], Y[HeroInfo.playerid()]);
		Hero->UserName = FString(HeroInfo.playername().c_str());
		Hero->PlayerID = HeroInfo.playerid(); // Important

		GI->AllCharacters.Add(Hero);

		if (HeroInfo.playerid() == GI->SelfID)
		{
			GI->HeroCharacter = Hero;
			Print(TEXT("Create Self Character"));

			// Fix Later
			// Should Possess After Begin Play Message
			/*
			auto PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PC)
			{
				PC->Possess(Hero);
			}
			*/
		}
	}

	// GI->Send(MType::MapLoadedRequest, nullptr, 0);
	// Print(TEXT("Load Map Done"));
}

void AParagonRoyalGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AHeroCharacterBase* AParagonRoyalGameModeBase::SpawnHeroAt(int HeroID, int SkinID, float X, float Y)
{
	AHeroCharacterBase* Result = nullptr;
	auto World = GetWorld();

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FRotator InitRot(0, 0, 0);

	switch (HeroID)
	{
	case 1:
	{
		float Height = 88.2f;
		if (SkinID >= 1 && SkinID <= AuroraBpWithSkins.Num())
		{
			Result = World->SpawnActor<AHeroCharacterAurora>(AuroraBpWithSkins[SkinID - 1], FVector(X, Y, Height), InitRot, ActorSpawnParams);
		}
	}break;

	case 2:
	{
		float Height = 98.2f;
		if (SkinID >= 1 && SkinID <= FengMaoBpWithSkins.Num())
		{
			Result = World->SpawnActor<AHeroCharacterFengMao>(FengMaoBpWithSkins[SkinID - 1], FVector(X, Y, Height), InitRot, ActorSpawnParams);
		}
	}break;

	case 3:
	{
		float Height = 118.2f;
		if (SkinID >= 1 && SkinID <= GruxBpWithSkins.Num())
		{
			Result = World->SpawnActor<AHeroCharacterGrux>(GruxBpWithSkins[SkinID - 1], FVector(X, Y, Height), InitRot, ActorSpawnParams);
		}
	}break;

	case 4:
	{
		float Height = 88.2f;
		if (SkinID >= 1 && SkinID <= SparrowBpWithSkins.Num())
		{
			Result = World->SpawnActor<AHeroCharacterSparrow>(SparrowBpWithSkins[SkinID - 1], FVector(X, Y, Height), InitRot, ActorSpawnParams);
		}
	}break;

	default:
	{

	}break;
	}

	return Result;
}

void AParagonRoyalGameModeBase::IntroDoneMapReady()
{
	if (GI)
	{
		auto PC = Cast<AHeroPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC)
		{
			PC->ShowWaitingUI();
		}

		GI->Send(MType::MapLoadedRequest, nullptr, 0);
		Print(TEXT("Map Load Done"));
	}
}