// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Kismet/GameplayStatics.h"

#include "../UI/SelfHeadBarUserWidget.h"

#include "HeadBarActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARAGONROYAL_API UHeadBarActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHeadBarActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Update(float Value);
	void SetName(const FString& HeadName);
	void SetHidden(bool IsHidden);
	void Die();

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USelfHeadBarUserWidget> HeadBarClass;

protected:
	UPROPERTY(BlueprintReadWrite)
	USelfHeadBarUserWidget* HeadBar = nullptr;

private:
	float TargetValue = 1.0f;

public:
	float OffsetZ = 128.0f;
};
