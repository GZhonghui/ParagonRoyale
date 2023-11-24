// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AHeroCharacterBase::AHeroCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1200.0f; // The camera follows at this distance behind the character	
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = false;
	SpringArm->SetRelativeRotation(FRotator(300.0f, 0.0f, 0.0f));

	// Create a follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm	

	if (!HeadBarComponent)
	{
		HeadBarComponent = CreateDefaultSubobject<UHeadBarActorComponent>(TEXT("HeadBar"));
	}

	auto Movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (Movement)
	{
		Movement->MaxWalkSpeed = 400.0f;
	}
}

// Called when the game starts or when spawned
void AHeroCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Not safe to use Player ID before BeginPlay

	// Mouse Over Highlight
	GetMesh()->OnBeginCursorOver.AddDynamic(this, &AHeroCharacterBase::OnCursorEnter);
	GetMesh()->OnEndCursorOver.AddDynamic(this, &AHeroCharacterBase::OnCursorLeave);

	if (ShadowClass)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		Shadow = GetWorld()->SpawnActor<AShadowActor>(ShadowClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);

		auto Capsule = Cast<UCapsuleComponent>(Shadow->GetComponentByClass(UCapsuleComponent::StaticClass()));
		if (Capsule)
		{
			Capsule->OnComponentBeginOverlap.AddDynamic(this, &AHeroCharacterBase::OnOverlapBegin);
			Capsule->OnComponentEndOverlap.AddDynamic(this, &AHeroCharacterBase::OnOverlapEnd);
		}
	}

	// Add Collision
	/*
	auto Capsule = Cast<UCapsuleComponent>(GetRootComponent());
	if (Capsule)
	{
		Capsule->OnComponentBeginOverlap.AddDynamic(this, &AHeroCharacterBase::OnOverlapBegin);
		Capsule->OnComponentEndOverlap.AddDynamic(this, &AHeroCharacterBase::OnOverlapEnd);
	}
	*/

	// Headbar, Should in Tick
	/*
	if (IsLocalPlayer())
	{
		if (HeadBarComponent)
		{
			HeadBarComponent->Die();
		}
	}
	*/

	// Set Init Lookat Target
	FVector Target = GetActorLocation() + GetActorForwardVector();
	LookatTarget = FVector2D(Target.X, Target.Y);

	// Init Turn
	NetSimulatedTurnStartPoint = LookatTarget;
	NetSimulatedTurnEndPoint = NetSimulatedTurnStartPoint;

	// Init Move
	auto InitLocation = GetActorLocation();
	NetSimulatedMoveStartPoint = FVector2D(InitLocation.X, InitLocation.Y);
	NetSimulatedMoveEndPoint = NetSimulatedMoveStartPoint;

	ShadowLocation.X = InitLocation.X;
	ShadowLocation.Y = InitLocation.Y;

	if (GI)
	{
		// GI->OnPlayerMove.AddUObject(this, &AHeroCharacterBase::OnNetMove);
		
		GI->OnStepBegin.AddUObject(this, &AHeroCharacterBase::OnStepStart); // useless
		GI->OnHaveClientStep.AddUObject(this, &AHeroCharacterBase::OnHaveStepMessage);
	}
}

// Called every frame
void AHeroCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	OneStepTimer += DeltaTime * 1e3; // ms

	LastSendSkillPassed += DeltaTime;

	auto ThisLocation = GetActorLocation();

	// For Shadow
	{
		const float FrameLong = static_cast<float>(GI->FrameLong);
		float StepProgress = OneStepTimer / FrameLong;

		auto NowMove = FMath::Lerp(NetSimulatedMoveStartPoint, NetSimulatedMoveEndPoint, StepProgress);
		ShadowLocation = FVector2D(NowMove.X, NowMove.Y);

		if (Shadow)
		{
			auto Location = Shadow->GetActorLocation();
			Shadow->SetActorLocation(FVector(ShadowLocation.X, ShadowLocation.Y, Location.Z));
		}
	}

	if (IsLocalPlayer())
	{
		if (HeadBarComponent)
		{
			HeadBarComponent->Die();

			// For test
			// HeadBarComponent->Update(HealthNow / HealthMax);
			// HeadBarComponent->SetName(UserName);
		}

		InWhichBlockNow(GI->LocalPlayerNowBlockX, GI->LocalPlayerNowBlockY);

		if (AllowTurn && !IsDead)
		{
			LookatOnPlane(LookatTarget.X, LookatTarget.Y);
		}
		else
		{
			LookatOnPlane(LookatTargetLimited.X, LookatTargetLimited.Y);
		}

		MoveSpeed = GetVelocity().Size();

		// Move Shadow
	}
	else
	{
		if (HeadBarComponent)
		{
			HeadBarComponent->Update(HealthNow / HealthMax);
			HeadBarComponent->SetName(UserName);
		}

		const float FrameLong = static_cast<float>(GI->FrameLong);
		float StepProgress = OneStepTimer / FrameLong;

		auto NowTurn = FMath::Lerp(NetSimulatedTurnStartPoint, NetSimulatedTurnEndPoint, StepProgress);
		LookatOnPlane(NowTurn.X, NowTurn.Y);

		// auto NowMove = FMath::Lerp(NetSimulatedMoveStartPoint, NetSimulatedMoveEndPoint, StepProgress);
		// SetLocationOnPlane(NowMove.X, NowMove.Y);

		SetLocationOnPlane(ShadowLocation.X, ShadowLocation.Y);

		auto TargetMoveSpeed = 1000.0f * (NetSimulatedMoveEndPoint - NetSimulatedMoveStartPoint).Size() / FrameLong; // ms to s
		MoveSpeed += DeltaTime * (TargetMoveSpeed - MoveSpeed) * 10.0f;

		if (AllowView)
		{
			int X, Y;
			InWhichBlockNow(X, Y);
			if (GI->BlockVisiable[X + 64][Y + 64])
			{
				SetActorHiddenInGame(false);
				if (HeadBarComponent)
				{
					HeadBarComponent->SetHidden(false);
				}
			}
			else
			{
				SetActorHiddenInGame(true);
				if (HeadBarComponent)
				{
					HeadBarComponent->SetHidden(true);
				}
			}
		}
	}

	TickSing(DeltaTime);

	if (!IsDead && !Won && AllowDamage)
	{
		TickDamge(DeltaTime);
	}

	if (HealthNow < 1.0f)
	{
		/*
		if (HeadBarComponent)
		{
			HeadBarComponent->Die();
		}
		*/

		if (IsLocalPlayer() && !IsDead)
		{
			if (AnimDie)
			{
				PlayAnimMontage(AnimDie);
			}
			if (HeadBarComponent)
			{
				HeadBarComponent->Die();
			}
			IsDead = true;
		}

		if (!IsLocalPlayer())
		{
			// Lost Connection
			if (ContinuousEmptyStep >= 10)
			{
				IsDead = true;
				ShouldDestroy = true;
			}
		}
	}

	// Only For Simulated Player
	// Wont delete local player
	if (ShouldDestroy && !DoDestroyed)
	{
		DoDestroyed = true;

		GI->LastPlayer -= 1;
		Print(FString::Printf(TEXT("Player %d Died"), PlayerID));

		if (Shadow)
		{
			Shadow->Destroy();
			Shadow = nullptr;
		}
		if (HeadBarComponent)
		{
			HeadBarComponent->Die();
		}

		// Crash
		// Destroy();

		auto NowLocation = GetActorLocation();
		NowLocation.Z = -1000.0f;
		SetActorLocation(NowLocation);

		OnMeDie.Broadcast(UserName);
	}
}

// Called to bind functionality to input
void AHeroCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHeroCharacterBase::GetInfoFromInstance(int InputHeroID)
{
	if (GI && GI->HeroInfos.Contains(InputHeroID))
	{
		const FHeroInfo& Info = GI->HeroInfos[InputHeroID];
		HealthNow = Info.HealthMax;
		HealthMax = Info.HealthMax;
		HealthRecover = Info.HealthRecover;

		ManaNow = Info.ManaMax;
		ManaMax = Info.ManaMax;
		ManaRecover = Info.ManaRecover;

		for (int i = 1; i <= 5; i += 1)
		{
			SkillsID[i] = Info.Skills[i];
		}

		HeroID = InputHeroID;
	}
}

void AHeroCharacterBase::StartSing(float Time, bool AllowMoveInThis, bool AllowTurnInThis)
{
	if (!AllowSkill) return;

	SingRemain = Time;
	SingTotal = Time;

	AllowMove = AllowMoveInThis;
	AllowTurn = AllowTurnInThis;

	AllowSkill = false;

	LookatTargetLimited = LookatTarget; // Update Turn
}

void AHeroCharacterBase::TickSing(float DeltaTime)
{
	SingRemain = std::max(-1.0f, SingRemain - DeltaTime);

	if (SingRemain < 0.0f)
	{
		AllowMove = true;
		AllowTurn = true;
		AllowSkill = true;

		AllowDamage = true;

		if (!AllowView)
		{
			AllowView = true;
			SetActorHiddenInGame(false);
			if (HeadBarComponent)
			{
				HeadBarComponent->SetHidden(false);
			}
		}	
	}
}

void AHeroCharacterBase::TickDamge(float DeltaTime)
{
	LastDamagePassed += DeltaTime;
	if (LastDamagePassed > AwayFromDamageNeedTime)
	{
		CauseDamage(PlayerID, -HealthRecover * DeltaTime);
	}
	SpendMana(-ManaRecover * DeltaTime);
	SpendMoney(-MoneyAddSpeed * DeltaTime);

	TArray<ADamageActorBase*> Gone;

	for (auto i = DamageActors.begin(); i != DamageActors.end(); ++i)
	{
		auto A = *i;
		if (!IsValid(A))
		{
			Gone.Add(A);
			continue;
		}
		else
		{
			float ThisDamage = 0.0f;
			CauseDamage(A->PlayerID, ThisDamage = A->Damage(this, DeltaTime));
			if (ThisDamage > 0.0f)
			{
				LastDamagePassed = 0.0f;
			}
		}
	}
	for (int i = 0; i < Gone.Num(); i += 1)
	{
		DamageActors.Remove(Gone[i]);
	}
}

void AHeroCharacterBase::CauseDamage(int FromWho, float Damge)
{
	if (HealthNow < Damge)
	{
		// Dead
	}
	HealthNow = FMath::Clamp(HealthNow - Damge, 0.0f, HealthMax);
}

bool AHeroCharacterBase::SpendMana(float ManaValue)
{
	if (ManaNow < ManaValue)
	{
		return false;
	}
	ManaNow = FMath::Clamp(ManaNow - ManaValue, 0.0f, ManaMax);
	return true;
}

bool AHeroCharacterBase::SpendMoney(float MoneyValue)
{
	if (MoneyNow < MoneyValue)
	{
		return false;
	}

	MoneyNow = FMath::Clamp(MoneyNow - MoneyValue, 0.0f, 999999.0f);
	return true;
}

void AHeroCharacterBase::AttachItem(int ItemID)
{
	// Also in Player Controller
	// Local Self: Update Character & Controller
	// Net Copy: Update Character Only
	switch (ItemID)
	{
	case 1:
	{
		HealthMax += 500.0f;
		HealthNow += 500.0f;
	}break;

	case 2:
	{
		HealthMax *= 2.0f;
		HealthNow *= 2.0f;
	}break;

	case 3:
	{
		HealthRecover *= 2.0f;
	}break;

	case 4:
	{
		ManaMax += 500.0f;
		ManaNow += 500.0f;
	}break;

	case 5:
	{
		ManaMax *= 2.0f;
		ManaNow *= 2.0f;
	}break;

	case 6:
	{
		ManaRecover *= 2.0f;
	}break;

	case 7:
	{

	}break;

	case 8:
	{
		DamageScale = 2.0f;
	}break;

	default:
	{
		Print(TEXT("Error Item ID!"));
	}break;
	}
}

bool AHeroCharacterBase::SkillLM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (ClientStep.actionsinthisstep_size()) return false;

	if (ActiveDirectly && !FromNet && LastSendSkillPassed > 0.3f)
	{
		LastSendSkillPassed = 0.0f;

		auto Action = ClientStep.add_actionsinthisstep();
		Action->set_type(1);

		auto SkillLocation = new PClientStep_Vec2;
		SkillLocation->set_x(Location.X);
		SkillLocation->set_y(Location.Y);
		Action->set_allocated_location(SkillLocation);

		auto SkillTarget = new PClientStep_Vec2;
		SkillTarget->set_x(Target.X);
		SkillTarget->set_y(Target.Y);
		Action->set_allocated_target(SkillTarget);
	}
	return false;
}

bool AHeroCharacterBase::SkillRM(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (ClientStep.actionsinthisstep_size()) return false;

	if (ActiveDirectly && !FromNet && LastSendSkillPassed > 0.3f)
	{
		LastSendSkillPassed = 0.0f;

		auto Action = ClientStep.add_actionsinthisstep();
		Action->set_type(2);

		auto SkillLocation = new PClientStep_Vec2;
		SkillLocation->set_x(Location.X);
		SkillLocation->set_y(Location.Y);
		Action->set_allocated_location(SkillLocation);

		auto SkillTarget = new PClientStep_Vec2;
		SkillTarget->set_x(Target.X);
		SkillTarget->set_y(Target.Y);
		Action->set_allocated_target(SkillTarget);
	}
	return false;
}

bool AHeroCharacterBase::SkillQ(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (ClientStep.actionsinthisstep_size()) return false;

	if (ActiveDirectly && !FromNet && LastSendSkillPassed > 0.3f)
	{
		LastSendSkillPassed = 0.0f;

		auto Action = ClientStep.add_actionsinthisstep();
		Action->set_type(3);

		auto SkillLocation = new PClientStep_Vec2;
		SkillLocation->set_x(Location.X);
		SkillLocation->set_y(Location.Y);
		Action->set_allocated_location(SkillLocation);

		auto SkillTarget = new PClientStep_Vec2;
		SkillTarget->set_x(Target.X);
		SkillTarget->set_y(Target.Y);
		Action->set_allocated_target(SkillTarget);
	}
	return false;
}

bool AHeroCharacterBase::SkillE(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (ClientStep.actionsinthisstep_size()) return false;

	if (ActiveDirectly && !FromNet && LastSendSkillPassed > 0.3f)
	{
		LastSendSkillPassed = 0.0f;

		auto Action = ClientStep.add_actionsinthisstep();
		Action->set_type(4);

		auto SkillLocation = new PClientStep_Vec2;
		SkillLocation->set_x(Location.X);
		SkillLocation->set_y(Location.Y);
		Action->set_allocated_location(SkillLocation);

		auto SkillTarget = new PClientStep_Vec2;
		SkillTarget->set_x(Target.X);
		SkillTarget->set_y(Target.Y);
		Action->set_allocated_target(SkillTarget);
	}
	return false;
}

bool AHeroCharacterBase::SkillSP(const FVector2D& Location, const FVector2D& Target, bool ActiveDirectly, bool FromNet)
{
	if (ClientStep.actionsinthisstep_size()) return false;

	if (ActiveDirectly && !FromNet && LastSendSkillPassed > 0.3f)
	{
		LastSendSkillPassed = 0.0f;

		auto Action = ClientStep.add_actionsinthisstep();
		Action->set_type(5);

		auto SkillLocation = new PClientStep_Vec2;
		SkillLocation->set_x(Location.X);
		SkillLocation->set_y(Location.Y);
		Action->set_allocated_location(SkillLocation);

		auto SkillTarget = new PClientStep_Vec2;
		SkillTarget->set_x(Target.X);
		SkillTarget->set_y(Target.Y);
		Action->set_allocated_target(SkillTarget);
	}
	return false;
}

void AHeroCharacterBase::CancelAllTargeting()
{

}

FVector AHeroCharacterBase::GetMouseWorldLocation()
{
	auto PC = Cast<APlayerController>(GetController());
	if (!PC) return FVector(0, 0, -1000.0f);

	float MouseX, MouseY;
	PC->GetMousePosition(MouseX, MouseY);

	FVector WorldLocation, WorldDirection;
	if (PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
	{
		FHitResult HitResult;
		auto TraceEnd = WorldLocation + WorldDirection * 1e4;

		GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Camera);

		if (HitResult.bBlockingHit)
		{
			return HitResult.Location;
		}
	}

	return FVector(0, 0, -1000.0f);
}

bool AHeroCharacterBase::IsLocalPlayer() const
{
	if (!GI) return false;
	return GI->SelfID == PlayerID;

	// Think about why
	// return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == this;
}

void AHeroCharacterBase::SetLocationOnPlane(float X, float Y)
{
	auto NowLocation = GetActorLocation();
	NowLocation.X = X;
	NowLocation.Y = Y;
	SetActorLocation(NowLocation);
}

void AHeroCharacterBase::LookatOnPlane(float X, float Y)
{
	auto ThisLocation = GetActorLocation();
	FVector Target = FVector(X, Y, ThisLocation.Z);

	auto Rotator = UKismetMathLibrary::FindLookAtRotation(ThisLocation, Target);
	SetActorRotation(Rotator);
}

void AHeroCharacterBase::InWhichBlockNow(int& X, int& Y)
{
	auto NowLocation = GetActorLocation();
	auto BlockWidth = GI->BlockWidthInMap;

	X = (int)((std::abs(NowLocation.X) + BlockWidth * 0.5f) / BlockWidth) * (NowLocation.X > 0 ? 1 : -1);
	Y = (int)((std::abs(NowLocation.Y) + BlockWidth * 0.5f) / BlockWidth) * (NowLocation.Y > 0 ? 1 : -1);
}

void AHeroCharacterBase::OnSetHeadbarShow(bool Show)
{
	if (HeadBarComponent)
	{
		HeadBarComponent->SetHidden(!Show);
	}
}

void AHeroCharacterBase::OnNetMove(int NetPlayerID, float X, float Y)
{
	// Print(FString::Printf(TEXT("Player %d at (%.1f,%.1f)"), NetPlayerID, X, Y));
	if (IsLocalPlayer())
	{
		// Move the shadow, maybe
	}
	else
	{
		if (PlayerID == NetPlayerID)
		{
			SetLocationOnPlane(X, Y);
		}
	}
}

void AHeroCharacterBase::OnNetTurn(int NetPlayerID, float X, float Y)
{
	if (IsLocalPlayer())
	{

	}
	else
	{
		if (PlayerID == NetPlayerID)
		{
			LookatTarget.X = X;
			LookatTarget.Y = Y;
		}
	}
}

void AHeroCharacterBase::OnStepStart(int NowStep)
{
	OneStepTimer = 0.0f;
	auto NowLocation = GetActorLocation();

	if (IsLocalPlayer() && NowStep >= 2)
	{
		// NowStep - 1 End
		ClientStep.set_stepindex(NowStep - 1); // Start from 1

		auto MoveInThisStep = new PClientStep_Vec2; // Bug
		MoveInThisStep->set_x(NowLocation.X);
		MoveInThisStep->set_y(NowLocation.Y);
		ClientStep.set_allocated_moveinthisstep(MoveInThisStep);

		auto TurnInThisStep = new PClientStep_Vec2;
		TurnInThisStep->set_x(LookatTarget.X);
		TurnInThisStep->set_y(LookatTarget.Y);
		ClientStep.set_allocated_turninthisstep(TurnInThisStep);

		ClientStep.set_lasthealthinthisstep(HealthNow);

		// must be true
		// ClientStep.actionsinthisstep_size() == 1;

		if (IsDead)
		{
			ClientStep.set_lasthealthinthisstep(0.0f);
			ClientStep.clear_actionsinthisstep();
		}

		TArray<uint8_t> Buffer;
		Buffer.SetNum(ClientStep.ByteSizeLong());
		ClientStep.SerializeToArray(Buffer.GetData(), ClientStep.ByteSizeLong());

		GI->Send(MType::ClientStep, Buffer.GetData(), ClientStep.ByteSizeLong());

		// Print(FString::Printf(TEXT("Send a %d"), NowStep - 1));

		ClientStep.clear_actionsinthisstep();
	}

	// NowStep Start
	if (NowStep >= 15)
	{
		int PlayStep = NowStep - 14;

		if (RecvedSteps.empty())
		{
			Print(TEXT("Network Unstable"));
		}
		else
		{
			auto ThisStep = RecvedSteps.front();
			RecvedSteps.pop();

			if (!IsDead)
			{
				NetSimulatedMoveStartPoint = NetSimulatedMoveEndPoint;
				NetSimulatedTurnStartPoint = NetSimulatedTurnEndPoint;

				// if empty? fix later
				// check if is a bug

				NetSimulatedMoveEndPoint = FVector2D(ThisStep.moveinthisstep().x(), ThisStep.moveinthisstep().y());
				NetSimulatedTurnEndPoint = FVector2D(ThisStep.turninthisstep().x(), ThisStep.turninthisstep().y());

				if (ThisStep.actionsinthisstep_size())
				{
					WillDoAction = ThisStep.actionsinthisstep(0);

					if (WillDoAction.type() >= 0)
					{
						FVector2D Location(WillDoAction.location().x(), WillDoAction.location().y());
						FVector2D Target(WillDoAction.target().x(), WillDoAction.target().y());

						switch (WillDoAction.type())
						{
						case 1:
						{
							SkillLM(Location, Target, true, true);
						}break;

						case 2:
						{
							SkillRM(Location, Target, true, true);
						}break;

						case 3:
						{
							SkillQ(Location, Target, true, true);
						}break;

						case 4:
						{
							SkillE(Location, Target, true, true);
						}break;

						case 5:
						{
							SkillSP(Location, Target, true, true);
						}break;

						default:
							break;
						}
					}
					else
					{
						int ItemID = WillDoAction.type() * -1;

						if (!AlreadyHaveItem.count(ItemID) && ItemID >= 1 && ItemID <= 8)
						{
							AlreadyHaveItem.insert(ItemID);

							AttachItem(ItemID);

							if (IsLocalPlayer())
							{
								OnRecvItem.Broadcast(ItemID);

								Print(FString::Printf(TEXT("Recv Item %d"), ItemID));
							}
							
							SpendMoney(GI->ItemInfos[ItemID].Cost);
						}
					}
				}

				if (!IsLocalPlayer())
				{
					if (ThisStep.lasthealthinthisstep() < -100.0f)
					{
						// Empty Step
					}
					else
					{
						HealthNow = ThisStep.lasthealthinthisstep();
						if (HealthNow < 1.0f)
						{
							IsDead = true;
							ShouldDestroy = true;
						}
					}
				}
			}
		}
	}
}

void AHeroCharacterBase::OnHaveStepMessage(int NetPlayerID, int IsEmpty, FClientStep Step)
{
	if (PlayerID == NetPlayerID)
	{
		if (IsEmpty)
		{
			// Print(TEXT("Empty Message"));

			ContinuousEmptyStep += 1;

			RecvedLastStep.clear_actionsinthisstep();
			RecvedLastStep.set_lasthealthinthisstep(-101.0f);
			RecvedLastStep.set_stepindex(RecvedLastStep.stepindex() + 1);
			RecvedSteps.push(RecvedLastStep); // Repeat Last Step, Did nothing in this step
		}
		else
		{
			ContinuousEmptyStep = 0;

			PClientStep ReadyStep;
			ReadyStep.ParseFromArray(Step.StepBuffer.GetData(), Step.StepBuffer.Num());

			RecvedSteps.push(ReadyStep);

			RecvedLastStep = ReadyStep;
			RecvedLastStep.clear_actionsinthisstep(); // No Actions
			RecvedLastStep.set_lasthealthinthisstep(-101.0f);
		}
	}
}

void AHeroCharacterBase::OnCursorEnter(class UPrimitiveComponent* OtherComp)
{
	if (UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) != this)
	{
		GetMesh()->SetRenderCustomDepth(true);
	}
}

void AHeroCharacterBase::OnCursorLeave(class UPrimitiveComponent* OtherComp)
{
	GetMesh()->SetRenderCustomDepth(false);
}

void AHeroCharacterBase::OnOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADamageActorBase* DamageActor = Cast<ADamageActorBase>(OtherActor);
	if (DamageActor)
	{
		DamageActors.Add(DamageActor);
	}
}

void AHeroCharacterBase::OnOverlapEnd(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADamageActorBase* DamageActor = Cast<ADamageActorBase>(OtherActor);
	if (DamageActor && DamageActors.Contains(DamageActor))
	{
		DamageActors.Remove(DamageActor);
	}
}
