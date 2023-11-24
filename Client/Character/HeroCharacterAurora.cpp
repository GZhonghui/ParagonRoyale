// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroCharacterAurora.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
AHeroCharacterAurora::AHeroCharacterAurora()
{
	HeroID = 1;
}

void AHeroCharacterAurora::BeginPlay()
{
	Super::BeginPlay();

	GetInfoFromInstance(HeroID);
}

void AHeroCharacterAurora::Tick(float DeltaTime)
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
		case 2:
		{
			if (DecalRMArrowFront)
			{
				DecalRMArrowFront->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 1220.0f);
				DecalRMArrowFront->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
			if (DecalRMArrowBack)
			{
				DecalRMArrowBack->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 600.0f);
				DecalRMArrowBack->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
		}break;

		case 3:
		{
			if (DecalQArrowFront)
			{
				DecalQArrowFront->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 1520.0f);
				DecalQArrowFront->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));

			}
			if (DecalQArrowBack)
			{
				DecalQArrowBack->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 700.0f);
				DecalQArrowBack->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
		}break;

		case 4:
		{
			if (DecalECenter)
			{
				DecalECenter->SetActorLocation(HeroLocation);

			}
			if (DecalETarget)
			{
				auto GroundMouse = MouseLocation;
				GroundMouse.Z = 0;

				auto GroundHero = HeroLocation;
				GroundHero.Z = 0;

				auto GroundForward = (MouseLocation - HeroLocation).GetSafeNormal();
				auto GroundLength = (MouseLocation - HeroLocation).Size();
				
				DecalETarget->SetActorLocation(GroundHero + GroundForward * std::min(800.0f, GroundLength));
				DecalETarget->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
		}break;

		default: break;
		}
	}
}

bool AHeroCharacterAurora::SkillLM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		if (AnimLM)
		{
			PlayAnimMontage(AnimLM);
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/aurora_LM_Cue.aurora_LM_Cue'"));
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

		Super::SkillLM(Location, Target, true, false); // Send to Server

		return true;
	}
}

bool AHeroCharacterAurora::SkillRM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && ProjectileRMClass)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			auto SpawnLocation = FVector(Location.X, Location.Y, 88.0f);
			auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			auto SpawnProjectileLocation = SpawnRotation.Vector() * 80 + SpawnLocation;

			if (AnimRM)
			{
				PlayAnimMontage(AnimRM);
				USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/aurora_RM_Cue.aurora_RM_Cue'"));
				if (fireSound) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
				}
			}

			auto A = World->SpawnActor<ADamageActorProjectileAuroraRM>(ProjectileRMClass, SpawnProjectileLocation, SpawnRotation, ActorSpawnParams);
			if (A)
			{
				A->PlayerID = PlayerID;
				A->Owner = this;
				A->DamageScale = DamageScale;
			}
		}

		OnReleaseSkill.Broadcast(2);

		return true;
	}
	else
	{
		if (ActiveDirectly)
		{
			CancelAllTargeting();

			Super::SkillRM(Location, Target, true, false);

			return true;
		}
		else
		{
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/aurora_ready_Cue.aurora_ready_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
			}
			if (TargetingSkill == 2)
			{
				CancelAllTargeting();
			}
			else
			{
				CancelAllTargeting();
				TargetingSkill = 2;

				DecalRMArrowFront = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				DecalRMArrowBack = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				if (DecalRMArrowFront)
				{
					DecalRMArrowFront->SetDecalMaterial(DecalMaterialRMArrowFront);
					DecalRMArrowFront->GetDecal()->DecalSize = FVector(480, 80, 100);
					DecalRMArrowFront->SetActorScale3D(FVector(1, 1, 1));
					DecalRMArrowFront->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
				if (DecalRMArrowBack)
				{
					DecalRMArrowBack->SetDecalMaterial(DecalMaterialRMArrowBack);
					DecalRMArrowBack->GetDecal()->DecalSize = FVector(480, 540, 100);
					DecalRMArrowBack->SetActorScale3D(FVector(1, 1, 1));
					DecalRMArrowBack->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
			}

			return false;
		}
	}
}

bool AHeroCharacterAurora::SkillQ(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticQClass)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SkillLocation = FVector(Location.X, Location.Y, 0.1f); // HARD CODE
			auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			auto A = World->SpawnActor<ADamageActorStaticAuroraQ>(StaticQClass, SkillLocation, SpawnRotation, ActorSpawnParams);
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/aurora_Q_Cue.aurora_Q_Cue'"));
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
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/aurora_ready_Cue.aurora_ready_Cue'"));
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

				DecalQArrowFront = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				DecalQArrowBack = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				if (DecalQArrowFront)
				{
					DecalQArrowFront->SetDecalMaterial(DecalMaterialQArrowFront);
					DecalQArrowFront->GetDecal()->DecalSize = FVector(480, 180, 480);
					DecalQArrowFront->SetActorScale3D(FVector(1, 1, 1));
					DecalQArrowFront->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
				if (DecalQArrowBack)
				{
					DecalQArrowBack->SetDecalMaterial(DecalMaterialQArrowBack);
					DecalQArrowBack->GetDecal()->DecalSize = FVector(480, 640, 480);
					DecalQArrowBack->SetActorScale3D(FVector(1, 1, 1));
					DecalQArrowBack->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
			}
			return false;
		}
	}
}

bool AHeroCharacterAurora::SkillE(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticEClass)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SkillLocation = FVector(Target.X, Target.Y, 0.1f); // HARD CODE
			auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			auto A = World->SpawnActor<ADamageActorStaticAuroraE>(StaticEClass, SkillLocation, SpawnRotation, ActorSpawnParams);
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/aurora_E_Cue.aurora_E_Cue'"));
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
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/aurora_ready_Cue.aurora_ready_Cue'"));
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

				DecalECenter = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				DecalETarget = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				if (DecalECenter)
				{
					DecalECenter->SetDecalMaterial(DecalMaterialECenter);
					DecalECenter->GetDecal()->DecalSize = FVector(480, 800, 800);
					DecalECenter->SetActorScale3D(FVector(1, 1, 1));
					DecalECenter->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
				if (DecalETarget)
				{
					DecalETarget->SetDecalMaterial(DecalMaterialETarget);
					DecalETarget->GetDecal()->DecalSize = FVector(480, 640, 640);
					DecalETarget->SetActorScale3D(FVector(1, 1, 1));
					DecalETarget->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
			}

			return false;
		}
	}
}

bool AHeroCharacterAurora::SkillSP(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticSPClass)
		{
			FActorSpawnParameters ActorSpawnParams;

			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// No Collision, the location is not important
			auto A = World->SpawnActor<ADamageActorStaticAuroraSP>(StaticSPClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
			if (A)
			{
				A->HeroAurora = this;

				A->PlayerID = PlayerID;
				A->Owner = this;
				A->DamageScale = DamageScale;
			}

			if (AnimSP)
			{
				PlayAnimMontage(AnimSP);
				USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/aurora_SP_Cue.aurora_SP_Cue'"));
				if (fireSound) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
				}
			}

			AllowDamage = false;

			StartSing(6.0f, false, false);
		}

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

void AHeroCharacterAurora::CancelAllTargeting()
{
	TargetingSkill = 0;
	if (DecalRMArrowFront)
	{
		DecalRMArrowFront->Destroy();
		DecalRMArrowFront = nullptr;
	}
	if (DecalRMArrowBack)
	{
		DecalRMArrowBack->Destroy();
		DecalRMArrowBack = nullptr;
	}

	if (DecalQArrowFront)
	{
		DecalQArrowFront->Destroy();
		DecalQArrowFront = nullptr;
	}
	if (DecalQArrowBack)
	{
		DecalQArrowBack->Destroy();
		DecalQArrowBack = nullptr;
	}

	if (DecalECenter)
	{
		DecalECenter->Destroy();
		DecalECenter = nullptr;
	}
	if (DecalETarget)
	{
		DecalETarget->Destroy();
		DecalETarget = nullptr;
	}
}
