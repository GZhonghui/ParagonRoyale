// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroCharacterSparrow.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
AHeroCharacterSparrow::AHeroCharacterSparrow()
{
	HeroID = 4;
}

void AHeroCharacterSparrow::BeginPlay()
{
	Super::BeginPlay();

	GetInfoFromInstance(HeroID);
}

void AHeroCharacterSparrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			for (int i = 0; i < 5; i += 1)
			{
				FRotator Rot(0, 15.0f * (i - 2), 0);
				auto Forward = Rot.RotateVector(HeroForward.GetSafeNormal());

				if (DecalRMArrowFront[i])
				{
					DecalRMArrowFront[i]->SetActorLocation(HeroLocation + Forward * 840.0f);
					DecalRMArrowFront[i]->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f + 15.0f * (i - 2), 0));

				}
				if (DecalRMArrowBack[i])
				{
					DecalRMArrowBack[i]->SetActorLocation(HeroLocation + Forward * 420.0f);
					DecalRMArrowBack[i]->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f + 15.0f * (i - 2), 0));
				}
			}
		}break;

		case 3:
		{
			for (int i = 0; i < 5; i += 1)
			{
				FRotator Rot(0, 15.0f * (i - 2), 0);
				auto Forward = Rot.RotateVector(HeroForward.GetSafeNormal());

				if (DecalQ[i])
				{
					float HeroMouseDistance = FVector2D(HeroLocation.X - MouseLocation.X, HeroLocation.Y - MouseLocation.Y).Size();

					DecalQ[i]->SetActorLocation(HeroLocation + Forward * std::max(HeroMouseDistance, 640.0f));
					DecalQ[i]->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f + 15.0f * (i - 2), 0));

				}
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

				DecalETarget->SetActorLocation(GroundHero + GroundForward * std::min(1080.0f, GroundLength));
				DecalETarget->SetActorRotation(FRotator(90, HeroRotation.Yaw + 90.0f, 0));
			}
		}break;

		default: break;
		}
	}
}

bool AHeroCharacterSparrow::SkillLM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && ProjectileLM)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// auto SpawnArrowLocation = GetMesh()->GetSocketLocation(FName(TEXT("BowEmitterSocket")));
			// auto SpawnArrowRotation = GetMesh()->GetSocketRotation(FName(TEXT("BowEmitterSocket")));

			auto SpawnArrowLocation = FVector(Location.X, Location.Y, 88.0f);
			auto SpawnArrowRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			if (AnimLM) {
				PlayAnimMontage(AnimLM);
				USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/sparrow_LM_Cue.sparrow_LM_Cue'"));
				if (fireSound) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
				}
			} 
			auto A = Cast<ADamageActorProjectileSparrowLM>(World->SpawnActor<ADamageActorProjectileSparrowLM>(ProjectileLM, SpawnArrowLocation, SpawnArrowRotation, ActorSpawnParams));
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

bool AHeroCharacterSparrow::SkillRM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && ProjectileRM)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// auto SpawnArrowLocation = GetMesh()->GetSocketLocation(FName(TEXT("BowEmitterSocket")));
			// auto SpawnArrowRotation = GetMesh()->GetSocketRotation(FName(TEXT("BowEmitterSocket")));

			// auto SpawnArrowLocation = GetActorLocation();
			// auto SpawnArrowRotation = GetActorRotation();

			auto SpawnArrowLocation = FVector(Location.X, Location.Y, 88.0f);
			auto SpawnArrowRotation = FVector(Target.X - Location.X, Target.Y - Location.Y, 0.0f).Rotation();

			auto SpawnArrowForward = SpawnArrowRotation.Vector();
			SpawnArrowForward.Z = 0;

			if (AnimRM) {
				PlayAnimMontage(AnimRM);
				USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/sparrow_RM2_Cue.sparrow_RM2_Cue'"));
				if (fireSound) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, GetActorLocation());
				}
			} 
			for (int i = 0; i < 5; i += 1)
			{
				FRotator Rot(0, 15.0f * (i - 2), 0);
				auto Forward = Rot.RotateVector(SpawnArrowForward.GetSafeNormal());

				auto A = Cast<ADamageActorProjectileSparrowRM>(World->SpawnActor<ADamageActorProjectileSparrowRM>(ProjectileRM, SpawnArrowLocation, Forward.Rotation(), ActorSpawnParams));
				if (A)
				{
					A->PlayerID = PlayerID;
					A->Owner = this;
					A->DamageScale = DamageScale;
				}
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
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/sparrow_draw_mix_Cue.sparrow_draw_mix_Cue'"));
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

				for (int i = 0; i < 5; i += 1)
				{
					DecalRMArrowFront[i] = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
					DecalRMArrowBack[i] = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
					if (DecalRMArrowFront[i])
					{
						DecalRMArrowFront[i]->SetDecalMaterial(DecalMaterialRMArrowFront);
						DecalRMArrowFront[i]->GetDecal()->DecalSize = FVector(480, 60, 80);
						DecalRMArrowFront[i]->SetActorScale3D(FVector(1, 1, 1));
						DecalRMArrowFront[i]->SetActorRotation(FRotator(90, 0, 0));
					}
					if (DecalRMArrowBack[i])
					{
						DecalRMArrowBack[i]->SetDecalMaterial(DecalMaterialRMArrowBack);
						DecalRMArrowBack[i]->GetDecal()->DecalSize = FVector(480, 360, 80);
						DecalRMArrowBack[i]->SetActorScale3D(FVector(1, 1, 1));
						DecalRMArrowBack[i]->SetActorRotation(FRotator(90, 0, 0));
					}
				}
			}

			return false;
		}
	}
}

bool AHeroCharacterSparrow::SkillQ(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticQ)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			auto HeroLocation = FVector(Location.X, Location.Y, 0.0f);
			auto MouseLocation = FVector(Target.X, Target.Y, 0.0f);

			// auto HeroRotation = UKismetMathLibrary::FindLookAtRotation(HeroLocation, MouseLocation);
			auto HeroRotation = (MouseLocation - HeroLocation).Rotation();
			auto HeroForward = HeroRotation.Vector();

			for (int i = 0; i < 5; i += 1)
			{
				FRotator Rot(0, 15.0f * (i - 2), 0);
				auto Forward = Rot.RotateVector(HeroForward.GetSafeNormal());

				float HeroMouseDistance = FVector2D(HeroLocation.X - MouseLocation.X, HeroLocation.Y - MouseLocation.Y).Size();

				auto SpawnLocation = HeroLocation + Forward * std::max(HeroMouseDistance, 640.0f);

				auto A = Cast<ADamageActorStaticSparrowQ>(World->SpawnActor<ADamageActorStaticSparrowQ>(StaticQ, SpawnLocation, Forward.Rotation(), ActorSpawnParams));
				if (A)
				{
					A->PlayerID = PlayerID;
					A->Owner = this;
					A->DamageScale = DamageScale;
				}
			}
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/sparrow_Q_Cue.sparrow_Q_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, MouseLocation);
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
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/sparrow_draw_mix_Cue.sparrow_draw_mix_Cue'"));
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

				for (int i = 0; i < 5; i += 1)
				{
					DecalQ[i] = GetWorld()->SpawnActor<ADecalActor>(FVector(), FRotator());
					if (DecalQ[i])
					{
						DecalQ[i]->SetDecalMaterial(DecalMaterialQ);
						DecalQ[i]->GetDecal()->DecalSize = FVector(480, 200, 200);
						DecalQ[i]->SetActorScale3D(FVector(1, 1, 1));
						DecalQ[i]->SetActorRotation(FRotator(90, 0, 0));
					}
				}
			}

			return false;
		}
	}
}

bool AHeroCharacterSparrow::SkillE(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticEClassBugFix)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SkillLocation = FVector(Target.X, Target.Y, 0.1f); // HARD CODE

			auto A = Cast<ADamageActorStaticSparrowE>(World->SpawnActor<ADamageActorStaticSparrowE>(StaticEClassBugFix, SkillLocation, GetActorRotation(), ActorSpawnParams));
			if (A)
			{
				A->PlayerID = PlayerID;
				A->Owner = this;
				A->DamageScale = DamageScale;
			}
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/sparrow_E_Cue.sparrow_E_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, SkillLocation);
			}
		}

		OnReleaseSkill.Broadcast(4);

		return true;
	}
	else
	{
		// Fix Bug

		CancelAllTargeting();

		Super::SkillE(Location, Target, true, false);

		return true;

		/*
		if (ActiveDirectly)
		{
			CancelAllTargeting();

			Super::SkillE(Location, Target, true, false);

			return true;
		}
		else
		{
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/sparrow_draw_mix_Cue.sparrow_draw_mix_Cue'"));
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
					DecalECenter->SetDecalMaterial(DecalMaterialECenterBugFix);
					DecalECenter->GetDecal()->DecalSize = FVector(480, 1080, 1080);
					DecalECenter->SetActorScale3D(FVector(1, 1, 1));
					DecalECenter->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));

				if (DecalETarget)
				{
					DecalETarget->SetDecalMaterial(DecalMaterialETargetBugFix);
					DecalETarget->GetDecal()->DecalSize = FVector(480, 320, 320);
					DecalETarget->SetActorScale3D(FVector(1, 1, 1));
					DecalETarget->SetActorRotation(FRotator(90, 0, 0));
				}
				else Print(TEXT("Create Failed"));
				
			}

			return false;
		}
		*/
	}
}

bool AHeroCharacterSparrow::SkillSP(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (FromNet)
	{
		auto World = GetWorld();
		if (World && StaticSP)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			auto HeroLocation = FVector(Location.X, Location.Y, 0.0f);
			auto MouseLocation = FVector(Target.X, Target.Y, 0.0f);

			// auto HeroRotation = UKismetMathLibrary::FindLookAtRotation(HeroLocation, MouseLocation);
			auto HeroRotation = (MouseLocation - HeroLocation).Rotation();

			auto A = Cast<ADamageActorStaticSparrowSP>(World->SpawnActor<ADamageActorStaticSparrowSP>(StaticSP, HeroLocation, HeroRotation, ActorSpawnParams));
			if (A)
			{
				A->PlayerID = PlayerID;
				A->Owner = this;
				A->DamageScale = DamageScale;
			}
			USoundCue* fireSound = LoadObject<USoundCue>(this, TEXT("SoundCue'/Game/0_Main/sound/sparrow_SP_Cue.sparrow_SP_Cue'"));
			if (fireSound) {
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, HeroLocation);
			}
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

void AHeroCharacterSparrow::CancelAllTargeting()
{
	TargetingSkill = 0;
	for (int i = 0; i < 5; i += 1)
	{
		if (DecalRMArrowFront[i])
		{
			DecalRMArrowFront[i]->Destroy();
			DecalRMArrowFront[i] = nullptr;
		}
		if (DecalRMArrowBack[i])
		{
			DecalRMArrowBack[i]->Destroy();
			DecalRMArrowBack[i] = nullptr;
		}

		if (DecalQ[i])
		{
			DecalQ[i]->Destroy();
			DecalQ[i] = nullptr;
		}
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
