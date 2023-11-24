// Fill out your copyright notice in the Description page of Project Settings.


#include "StylePickableActor.h"

#include "../Character/HeroCharacterBase.h"

// Sets default values
AStylePickableActor::AStylePickableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStylePickableActor::BeginPlay()
{
	Super::BeginPlay();

	auto BoxCollision = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AStylePickableActor::OnOverlapBegin);
		BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AStylePickableActor::OnOverlapEnd);
	}
	
}

// Called every frame
void AStylePickableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Picked)
	{
		Destroy();
	}

}

void AStylePickableActor::OnOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHeroCharacterBase* Hero = Cast<AHeroCharacterBase>(OtherActor);
	if (Hero)
	{
		if (!Picked)
		{
			Hero->SpendMoney(-100.0f);
			Picked = true;
		}
	}
}

void AStylePickableActor::OnOverlapEnd(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// None
}

