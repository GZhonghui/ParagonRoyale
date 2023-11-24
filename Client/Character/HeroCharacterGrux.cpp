// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroCharacterGrux.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
AHeroCharacterGrux::AHeroCharacterGrux()
{
	HeroID = 3;
}

void AHeroCharacterGrux::BeginPlay()
{
	Super::BeginPlay();

	GetInfoFromInstance(HeroID);
}

void AHeroCharacterGrux::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetingSkill)
	{
		auto HeroLocation = GetActorLocation();
		auto HeroForward = GetActorForwardVector();
		auto HeroRotation = GetActorRotation();

		switch (TargetingSkill)
		{
		case 2:
		{
			if (DecalRMArrowFront)
			{
				DecalRMArrowFront->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 840.0f);
				DecalRMArrowFront->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));

			}
			if (DecalRMArrowBack)
			{
				DecalRMArrowBack->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 420.0f);
				DecalRMArrowBack->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
		}break;

		case 3:
		{
			if (DecalQ)
			{
				DecalQ->SetActorLocation(HeroLocation + HeroForward.GetSafeNormal() * 420.0f);
				DecalQ->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
		}

		default: break;
		}
	}
}

bool AHeroCharacterGrux::SkillLM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		if (AnimLM) {
		PlayAnimMontage(AnimLM);
		USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/grux_LM_Cue.grux_LM_Cue'"));
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

bool AHeroCharacterGrux::SkillRM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		if (!ProjectileRMClass) return true;

		auto World = GetWorld(); if (!World) return true;

		if (AnimRM) PlayAnimMontage(AnimRM);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// auto SpawnProjectileLocation = GetActorForwardVector() * 80 + GetActorLocation();
		auto SpawnProjectileLocation = FVector(Location.X, Location.Y, 88.0f);
		auto SpawnProjectileRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

		auto A = World->SpawnActor<ADamageActorProjectileGruxRM>(ProjectileRMClass, SpawnProjectileLocation, SpawnProjectileRotation, ActorSpawnParams);
		if (A)
		{
			A->PlayerID = PlayerID;
			A->Owner = this;
			A->DamageScale = DamageScale;
		}
		USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/grux_RM_Cue.grux_RM_Cue'"));
		if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
		}
		OnReleaseSkill.Broadcast(2);

		return true;
	}
	else
	{
		// Fix Bug
		CancelAllTargeting();

		Super::SkillRM(Location, Target, true, false);

		return true;

		/*
		if (ActiveDirectly)
		{
			CancelAllTargeting();

			Super::SkillRM(Location, Target, true, false);

			return true;
		}
		else
		{
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/grux_ready_Cue.grux_ready_Cue'"));
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
					DecalRMArrowFront->SetDecalMaterial(DecalMaterialRMArrowFrontBugFix);
					DecalRMArrowFront->GetDecal()->DecalSize = FVector(480, 60, 120);
					DecalRMArrowFront->SetActorScale3D(FVector(1, 1, 1));
					DecalRMArrowFront->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
				if (DecalRMArrowBack)
				{
					DecalRMArrowBack->SetDecalMaterial(DecalMaterialRMArrowBackBugFix);
					DecalRMArrowBack->GetDecal()->DecalSize = FVector(480, 360, 120);
					DecalRMArrowBack->SetActorScale3D(FVector(1, 1, 1));
					DecalRMArrowBack->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
				
			}

			return false;
		}
		*/
	}
}

bool AHeroCharacterGrux::SkillQ(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		if (!StaticQClass) return true;
		auto World = GetWorld(); if (!World) return true;

		if (AnimQ) PlayAnimMontage(AnimQ);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		auto SpawnLocation = FVector(Location.X, Location.Y, 88.0f);
		auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

		auto A = World->SpawnActor<ADamageActorStaticGruxQ>(StaticQClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (A)
		{
			A->PlayerID = PlayerID;
			A->Owner = this;
			A->DamageScale = 1.0f;
		}

		StartSing(2.33f, false, false);

		OnReleaseSkill.Broadcast(3);
		USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/grux_Q_Cue.grux_Q_Cue'"));
		if (fireSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
		}
		return true;
	}
	else
	{
		// Fix Bug
		CancelAllTargeting();

		Super::SkillQ(Location, Target, true, false);

		return true;

		/*
		if (ActiveDirectly)
		{
			CancelAllTargeting();

			Super::SkillQ(Location, Target, true, false);

			return true;
		}
		else
		{
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/grux_ready_Cue.grux_ready_Cue'"));
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

				DecalQ = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
				if (DecalQ)
				{
					DecalQ->SetDecalMaterial(DecalMaterialQBugFix);
					DecalQ->GetDecal()->DecalSize = FVector(480, 360, 360);
					DecalQ->SetActorScale3D(FVector(1, 1, 1));
					DecalQ->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
			}

			return false;
		}
		*/
	}
}

bool AHeroCharacterGrux::SkillE(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		if (!StaticEClass) return true;
		auto World = GetWorld(); if (!World) return true;

		if (AnimE){
		 	PlayAnimMontage(AnimE);
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/grux_E_Cue.grux_E_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
			}
		} 
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		auto SpawnLocation = FVector(Location.X, Location.Y, 88.0f);
		auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

		auto A = World->SpawnActor<ADamageActorStaticGruxE>(StaticEClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (A)
		{
			A->PlayerID = PlayerID;
			A->Owner = this;
			A->DamageScale = DamageScale;
		}

		StartSing(3.6f, false, false);

		OnReleaseSkill.Broadcast(4);

		return true;
	}
	else
	{
		CancelAllTargeting();

		Super::SkillE(Location, Target, true, false);

		return true;
	}
}

bool AHeroCharacterGrux::SkillSP(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && ProjectileSPClass)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			auto SpawnLocation = FVector(Location.X, Location.Y, 88.0f);
			auto SpawnRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			FRotator Rot(0, 90, 0);
			auto Forward = SpawnRotation.Vector();
			auto ActorLocation = SpawnLocation;

			for (int i = 0; i < 4; i += 1)
			{
				auto SpawnProjectileLocation = Forward * 80 + ActorLocation;

				FRotator ProjectileRot = UKismetMathLibrary::FindLookAtRotation(ActorLocation, SpawnProjectileLocation);

				auto A = World->SpawnActor<ADamageActorProjectileGruxSP>(ProjectileSPClass, SpawnProjectileLocation, ProjectileRot, ActorSpawnParams);
				if (A)
				{
					A->HeroGrux = this;
					A->InitForward = Forward;

					A->PlayerID = PlayerID;
					A->Owner = this;
					A->DamageScale = DamageScale;
				}

				Forward = Rot.RotateVector(Forward);
			}
		}
		USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/grux_SP_Cue.grux_SP_Cue'"));
		if (fireSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
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

void AHeroCharacterGrux::CancelAllTargeting()
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
	if (DecalQ)
	{
		DecalQ->Destroy();
		DecalQ = nullptr;
	}
}
