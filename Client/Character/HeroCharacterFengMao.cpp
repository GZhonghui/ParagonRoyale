// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroCharacterFengMao.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
AHeroCharacterFengMao::AHeroCharacterFengMao()
{
	HeroID = 2;
}

void AHeroCharacterFengMao::BeginPlay()
{
	Super::BeginPlay();

	GetInfoFromInstance(HeroID);
}

void AHeroCharacterFengMao::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update Decal
	if (TargetingSkill)
	{
		auto HeroLocation = GetActorLocation();
		auto HeroForward = GetActorForwardVector();
		auto HeroRotation = GetActorRotation();
		auto MouseLocation = GetMouseWorldLocation();

		switch (TargetingSkill)
		{
		case 3:
		{
			if (DecalQTarget)
			{
				auto GroundMouse = MouseLocation;
				GroundMouse.Z = 0;

				DecalQTarget->SetActorLocation(GroundMouse);
				DecalQTarget->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
		}break;

		case 4:
		{
			if (DecalEArrowFront)
			{
				DecalEArrowFront->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 1520.0f);
				DecalEArrowFront->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));

			}
			if (DecalEArrowBack)
			{
				DecalEArrowBack->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 700.0f);
				DecalEArrowBack->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
		}break;

		default: break;
		}
	}
}

bool AHeroCharacterFengMao::SkillLM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		if (AnimLM)
		{
			PlayAnimMontage(AnimLM);
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/fengmao_LM_Cue.fengmao_LM_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
			}
		}

		auto World = GetWorld();
		if (World && StaticLMClass)
		{
			auto AttackLocation = FVector(Location.X, Location.Y, 0.1f);
			auto AttackRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			auto A = World->SpawnActor<ADamageActorStaticAttack>(StaticLMClass, AttackLocation, AttackRotation, ActorSpawnParams);
			if (A)
			{
				A->PlayerID = PlayerID;
				A->Owner = this;
				A->DamageScale = DamageScale;
			}
		}

		OnReleaseSkill.Broadcast(1);

		return true;
	}
	else
	{
		CancelAllTargeting();

		Super::SkillLM(Location, Target, true, false);

		return true;
	}
}

bool AHeroCharacterFengMao::SkillRM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticRMClass)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			if (AnimRM)
			{
				PlayAnimMontage(AnimRM);
				
				USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/fengmao_RM_Cue.fengmao_RM_Cue'"));
				if (fireSound) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
				}
			}

			auto SpawnLocation = FVector(Location.X, Location.Y, 88.0f);
			auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			auto A = World->SpawnActor<ADamageActorStaticFengMaoRM>(StaticRMClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if (A)
			{
				A->HeroFengMao = this;

				A->PlayerID = PlayerID;
				A->Owner = this;
				A->DamageScale = DamageScale;
			}

			StartSing(6.0f, true, true);
		}

		OnReleaseSkill.Broadcast(2);

		return true;
	}
	else
	{
		CancelAllTargeting();

		Super::SkillRM(Location, Target, true, false);

		return true;
	}
}

bool AHeroCharacterFengMao::SkillQ(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticQClass)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			FVector SkillLocation = FVector(Target.X, Target.Y, 0.1f); // HARD CODE

			auto A = World->SpawnActor<ADamageActorStaticFengMaoQ>(StaticQClass, SkillLocation, SpawnRotation, ActorSpawnParams);
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/fengmao_Q_Cue.fengmao_Q_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, SkillLocation);
			}
			if (A)
			{
				A->PlayerID = PlayerID;
				A->Owner = this;
				A->DamageScale = DamageScale;
			}
		}

		OnReleaseSkill.Broadcast(3);

		return true;
	}
	else
	{
		if (ActiveDirectly)
		{
			CancelAllTargeting();

			Super::SkillQ(Location, Target, true, false);

			return true;
		}
		else
		{
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/fengmao_ready_Cue.fengmao_ready_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
			}
			if (TargetingSkill == 3)
			{
				CancelAllTargeting();
			}
			else
			{
				CancelAllTargeting();
				TargetingSkill = 3;

				DecalQTarget = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				if (DecalQTarget)
				{
					DecalQTarget->SetDecalMaterial(DecalMaterialQTarget);
					DecalQTarget->GetDecal()->DecalSize = FVector(480, 240, 240);
					DecalQTarget->SetActorScale3D(FVector(1, 1, 1));
					DecalQTarget->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
			}

			return false;
		}
	}
}

bool AHeroCharacterFengMao::SkillE(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticEClass)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SkillLocation = FVector(Location.X, Location.Y, 0.1f); // HARD CODE
			auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			auto A = World->SpawnActor<ADamageActorStaticFengMaoE>(StaticEClass, SkillLocation, SpawnRotation, ActorSpawnParams);
			if (A)
			{
				A->PlayerID = PlayerID;
				A->Owner = this;
				A->DamageScale = DamageScale;
			}

			if (AnimE) PlayAnimMontage(AnimE);

			if (AnimE) StartSing(AnimE->GetPlayLength(), false, false);
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/fengmao_E_Cue.fengmao_E_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
			}
		}

		OnReleaseSkill.Broadcast(4);

		return true;
	}
	else
	{
		if (ActiveDirectly)
		{
			CancelAllTargeting();

			Super::SkillE(Location, Target, true, false);

			return true;
		}
		else
		{
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/fengmao_ready_Cue.fengmao_ready_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
			}
			if (TargetingSkill == 4)
			{
				CancelAllTargeting();
			}
			else
			{
				CancelAllTargeting();
				TargetingSkill = 4;

				DecalEArrowFront = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				DecalEArrowBack = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				if (DecalEArrowFront)
				{
					DecalEArrowFront->SetDecalMaterial(DecalMaterialEArrowFront);
					DecalEArrowFront->GetDecal()->DecalSize = FVector(480, 180, 160);
					DecalEArrowFront->SetActorScale3D(FVector(1, 1, 1));
					DecalEArrowFront->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
				if (DecalEArrowBack)
				{
					DecalEArrowBack->SetDecalMaterial(DecalMaterialEArrowBack);
					DecalEArrowBack->GetDecal()->DecalSize = FVector(480, 640, 160);
					DecalEArrowBack->SetActorScale3D(FVector(1, 1, 1));
					DecalEArrowBack->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
			}
			return false;
		}
	}
}

bool AHeroCharacterFengMao::SkillSP(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		if (ParticleSPSmoke)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSPSmoke, GetActorTransform(), true);
		}

		AllowView = false;
		USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/fengmao_SP_Cue.fengmao_SP_Cue'"));
		if (fireSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
		}
		// SetHidden(true);
		SetActorHiddenInGame(true);
		if (HeadBarComponent)
		{
			HeadBarComponent->SetHidden(true);
		}
		StartSing(5.0f, true, true);

		OnReleaseSkill.Broadcast(5);

		return true;
	}
	else
	{
		CancelAllTargeting();

		Super::SkillSP(Location, Target, true, false);

		return true;
	}
}

void AHeroCharacterFengMao::CancelAllTargeting()
{
	TargetingSkill = 0;

	if (DecalQTarget)
	{
		DecalQTarget->Destroy();
		DecalQTarget = nullptr;
	}

	if (DecalEArrowFront)
	{
		DecalEArrowFront->Destroy();
		DecalEArrowFront = nullptr;
	}
	if (DecalEArrowBack)
	{
		DecalEArrowBack->Destroy();
		DecalEArrowBack = nullptr;
	}
}
