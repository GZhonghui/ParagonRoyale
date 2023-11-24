// Fill out your copyright notice in the Description page of Project Settings.


#include "ParagonRoyalStyleGameModeBase.h"

void AParagonRoyalStyleGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (GI)
	{
		// Generate Map
		int MapHalfSize = 30;

		const int* PlayerX = GI->PlayerInitLocationAsBlockX;
		const int* PlayerY = GI->PlayerInitLocationAsBlockY;

		int MapID = GI->MapSeed % (GI->GameMapNumber + 1);

		auto Map = GI->GameMap[MapID];

		FRandomStream MapRandom(GI->MapSeed);

		for (int i = -MapHalfSize; i <= MapHalfSize; i += 1)
		{
			for (int j = -MapHalfSize; j <= MapHalfSize; j += 1)
			{
				bool CanPlaceBlock = true;
				for (int x = 1; x <= 4; x += 1) // HARD CODE
				{
					if (i == PlayerX[x] && j == PlayerY[x])
					{
						CanPlaceBlock = false;
						break;
					}
				}

				if (CanPlaceBlock)
				{
					if (MapID)
					{
						int Type = 0;
						if (Map[i + MapHalfSize][j + MapHalfSize] == 1) // Block
						{
							Type = 99;
						}
						else if (Map[i + MapHalfSize][j + MapHalfSize] == 2) // Grass
						{
							Type = 72;
						}
						else if (Map[i + MapHalfSize][j + MapHalfSize] == 3) // Money
						{
							Type = 62;
						}
						PlaceAt(i, j, Type);
					}
					else
					{
						if (std::abs(i) == MapHalfSize || std::abs(j) == MapHalfSize)
						{
							PlaceAt(i, j, 99); // HARD CODE
						}
						else
						{
							PlaceAt(i, j, MapRandom.RandRange(0, 99));
						}
					}
				}
			}
		}

		Print(TEXT("Generate Map Done"));
	}
}

void AParagonRoyalStyleGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AParagonRoyalStyleGameModeBase::PlaceAt(int X, int Y, int Type)
{
	float BlockWidth = GI->BlockWidthInMap;

	auto World = GetWorld();

	FVector TargetLocation(X * BlockWidth, Y * BlockWidth, 0);
	FRotator TargetRotation(0, 0, 0);

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (Type >= 92 && BlockClass)
	{
		World->SpawnActor<AStyleBlockActor>(BlockClass, TargetLocation, TargetRotation, ActorSpawnParams);
	}
	else if (Type >= 64 && GrassClass)
	{
		auto G = World->SpawnActor<AStyleGrassActor>(GrassClass, TargetLocation, TargetRotation, ActorSpawnParams);
		G->X = X;
		G->Y = Y;
	}
	else if (Type >= 60 && PickableClass)
	{
		auto G = World->SpawnActor<AStylePickableActor>(PickableClass, TargetLocation, TargetRotation, ActorSpawnParams);
	}
}