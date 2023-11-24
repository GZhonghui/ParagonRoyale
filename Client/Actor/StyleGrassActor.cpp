// Fill out your copyright notice in the Description page of Project Settings.


#include "StyleGrassActor.h"

// Sets default values
AStyleGrassActor::AStyleGrassActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStyleGrassActor::BeginPlay()
{
	Super::BeginPlay();

	/*
	auto RenderCube = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (RenderCube)
	{
		MiGrass = UMaterialInstanceDynamic::Create(RenderCube->GetMaterial(0), this);
		MiGrass->SetScalarParameterValue(FName(TEXT("Limit")), 1.0f);
		RenderCube->SetMaterial(0, MiGrass);
	}
	*/
}

// Called every frame
void AStyleGrassActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GI)
	{
		int PlayerX = GI->LocalPlayerNowBlockX;
		int PlayerY = GI->LocalPlayerNowBlockY;

		if (std::abs(X - PlayerX) <= 2 && std::abs(Y - PlayerY) <= 2)
		{
			// SetActorHiddenInGame(true);
			// MiGrass->SetScalarParameterValue(FName(TEXT("Limit")), 0.0f);
			GI->BlockVisiable[X + 64][Y + 64] = true;
		}
		else
		{
			// SetActorHiddenInGame(false);
			// MiGrass->SetScalarParameterValue(FName(TEXT("Limit")), 1.0f);
			GI->BlockVisiable[X + 64][Y + 64] = false;
		}
	}
}

