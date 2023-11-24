// Fill out your copyright notice in the Description page of Project Settings.


#include "HeadBarActorComponent.h"

// Sets default values for this component's properties
UHeadBarActorComponent::UHeadBarActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHeadBarActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (HeadBarClass && !HeadBar)
	{
		HeadBar = CreateWidget<USelfHeadBarUserWidget>(GetWorld(), HeadBarClass);
		if (HeadBar)
		{
			HeadBar->AddToViewport();
		}
	}
}


// Called every frame
void UHeadBarActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (HeadBar)
	{
		auto A = GetOwner();
		auto Location = A->GetActorLocation();
		Location.Z += OffsetZ;

		auto PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			FVector2D TargetScreen;
			PC->ProjectWorldLocationToScreen(Location, TargetScreen);
			HeadBar->SetPositionInViewport(TargetScreen);
		}

		auto Delta = 0.5f * DeltaTime; // Speed = 0.1
		auto NewValue = TargetValue;
		if (std::abs(TargetValue - HeadBar->Health->Percent) > Delta)
		{
			NewValue = HeadBar->Health->Percent + Delta * ((TargetValue > HeadBar->Health->Percent) ? 1 : -1);
		}
		HeadBar->Health->SetPercent(NewValue);
	}
}

void UHeadBarActorComponent::Update(float Value)
{
	if (HeadBar && HeadBar->Health)
	{
		TargetValue = FMath::Clamp(Value, 0.0f, 1.0f);
		// HeadBar->Health->SetPercent(FMath::Clamp(Value, 0.0f, 1.0f));
	}
}
void UHeadBarActorComponent::SetName(const FString& HeadName)
{
	if (HeadBar && HeadBar->NameTextBlock)
	{
		HeadBar->NameTextBlock->SetText(FText::FromString(HeadName));
	}
}

void UHeadBarActorComponent::SetHidden(bool IsHidden)
{
	if (HeadBar)
	{
		if (IsHidden)
		{
			HeadBar->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			HeadBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UHeadBarActorComponent::Die()
{
	if (HeadBar)
	{
		HeadBar->RemoveFromParent();
		HeadBar = nullptr;
	}
}

