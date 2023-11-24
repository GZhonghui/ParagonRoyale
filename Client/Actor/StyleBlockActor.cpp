// Fill out your copyright notice in the Description page of Project Settings.


#include "StyleBlockActor.h"

// Sets default values
AStyleBlockActor::AStyleBlockActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStyleBlockActor::BeginPlay()
{
	Super::BeginPlay();
	
	auto BoxCollision = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AStyleBlockActor::OnOverlapBegin);
		BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AStyleBlockActor::OnOverlapEnd);
	}
}

// Called every frame
void AStyleBlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStyleBlockActor::OnOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADamageActorBase* DamageActor = Cast<ADamageActorBase>(OtherActor);
	if (DamageActor)
	{
		DamageActor->Damage(this, 0.1f);
	}
}

void AStyleBlockActor::OnOverlapEnd(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// None
}

