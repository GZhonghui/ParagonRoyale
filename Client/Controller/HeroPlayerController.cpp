// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroPlayerController.h"

void AHeroPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(true);
	bEnableMouseOverEvents = true;

	// SetInputMode(FInputModeGameOnly()); // Why is this not working?
	SetInputMode(FInputModeGameAndUI());

	GameUI = CreateWidget<UGameUserWidget>(this, GameUIClass);
	if (GameUI)
	{
		auto SelfPawn = Cast<AHeroCharacterBase>(GetPawn()); // Actually Cant Get Pawn Now, not bind yet
		if (SelfPawn && SelfPawn->HeroID)
		{
			GameUI->SetSkills(SelfPawn->SkillsID); // Array, 5 Skills

			GameUI->UpdateHealth(SelfPawn->HealthNow, SelfPawn->HealthMax, SelfPawn->HealthRecover);
			GameUI->UpdateMana(SelfPawn->ManaNow, SelfPawn->ManaMax, SelfPawn->ManaRecover);

			SaveHeroID = SelfPawn->HeroID;
		}

		USelfShopItemUserWidget* ShopItems[] =
		{
			nullptr,
			GameUI->Item_01, GameUI->Item_02, GameUI->Item_03, GameUI->Item_04,
			GameUI->Item_05, GameUI->Item_06, GameUI->Item_07, GameUI->Item_08
		};
		for (int i = 1; i <= 8; i += 1)
		{
			ShopItems[i]->OnBuyMe.AddUObject(this, &AHeroPlayerController::BuyItem);
			ShopItems[i]->OnWantBuyMe.AddUObject(this, &AHeroPlayerController::OnWantBuySome);
		}

		// GameUI->AddToViewport();
	}	
}

void AHeroPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AHeroPlayerController::OnInputMoveForward);
	InputComponent->BindAxis("MoveRight", this, &AHeroPlayerController::OnInputMoveRight);

	InputComponent->BindAction("TAB", IE_Pressed, this, &AHeroPlayerController::OnInputTab);
	InputComponent->BindAction("LM", IE_Pressed, this, &AHeroPlayerController::OnInputSkillLMouse);
	InputComponent->BindAction("RM", IE_Pressed, this, &AHeroPlayerController::OnInputSkillRMouse);
	InputComponent->BindAction("Q", IE_Pressed, this, &AHeroPlayerController::OnInputSkillQ);
	InputComponent->BindAction("E", IE_Pressed, this, &AHeroPlayerController::OnInputSkillE);
	InputComponent->BindAction("SP", IE_Pressed, this, &AHeroPlayerController::OnInputSkillSpace);
	InputComponent->BindAction("Menu", IE_Pressed, this, &AHeroPlayerController::OnInputMenu);
}

void AHeroPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LastBuyPassed += DeltaTime;

	AddGameUIToViewPort();

	auto MouseLocation = GetMouseWorldLocation();

	auto SelfHero = Cast<AHeroCharacterBase>(GetPawn());
	if (GameUI && SelfHero)
	{
		// if (std::abs(MouseLocation.Z) < 1.0f)
		{
			SelfHero->LookatTarget = FVector2D(MouseLocation.X, MouseLocation.Y);
		}

		int RoomSize = GI->RoomSize;

		if (SelfHero->IsDead || GI->GetPlayerNumber() <= 1)
		{
			if (GameUI->GameResultPanel->Visibility != ESlateVisibility::Visible)
			{
				GameUI->GameResultPanel->SetVisibility(ESlateVisibility::Visible);
				GameUI->TextResultRank->SetText(FText::FromString(FString::Printf(TEXT("%d"), GI->GetPlayerNumber())));

				SelfHero->AllowMove = false;

				// The Last One Won the Game
				if (GI->GetPlayerNumber() <= 1)
				{
					SelfHero->Won = true;

					GameUI->TextResultYouDie->SetText(FText::FromString("You Survived"));
					GameUI->ImageResultChicken->SetVisibility(ESlateVisibility::Visible);
				}

				SetInputMode(FInputModeUIOnly()); // Game End
			}
		}

		// Has Pawn
		GameUI->GameRunning(true);

		GameUI->UpdateHealth(SelfHero->HealthNow, SelfHero->HealthMax, SelfHero->HealthRecover);
		GameUI->UpdateMana(SelfHero->ManaNow, SelfHero->ManaMax, SelfHero->ManaRecover);
		GameUI->UpdateMoney(static_cast<int>(SelfHero->MoneyNow));
		GameUI->UpdateSingState(SelfHero->SingRemain, SelfHero->SingTotal);

		// There is a bug, what if change to the same hero?
		// if (SelfHero->HeroID && SaveHeroID != SelfHero->HeroID) // Changed
		if (SelfHero != SaveHero)
		{
			SaveHero = SelfHero; // Save New Hero
			Print(TEXT("Chanaged Pawn"));

			GameUI->SetSkills(SelfHero->SkillsID); // Array, 5 Skills

			GameUI->UpdateHealth(SelfHero->HealthNow, SelfHero->HealthMax, SelfHero->HealthRecover);
			GameUI->UpdateMana(SelfHero->ManaNow, SelfHero->ManaMax, SelfHero->ManaRecover);

			SelfHero->OnReleaseSkill.AddUObject(this, &AHeroPlayerController::ReleaseSkill);
			SelfHero->OnRecvItem.AddUObject(this, &AHeroPlayerController::OnPawnRecvItem);

			SaveHeroID = SelfHero->HeroID; // Update Hero ID

			// may cause bug, fix later
			auto LGI = GI;
			for (int i = 0; i < LGI->AllCharacters.Num(); i += 1)
			{
				if (!IsValid(LGI->AllCharacters[i])) continue;

				AHeroCharacterBase* Hero = Cast<AHeroCharacterBase>(LGI->AllCharacters[i]);
				if (Hero)
				{
					Hero->OnMeDie.AddUObject(GameUI, &UGameUserWidget::ShowPlayerDie);
				}
			}
		}

		USelfShopItemUserWidget* ItemSlost[] =
		{
			nullptr,
			GameUI->Item_01, GameUI->Item_02, GameUI->Item_03, GameUI->Item_04,
			GameUI->Item_05, GameUI->Item_06, GameUI->Item_07, GameUI->Item_08
		};

		/*
		while (!BuyQueue.empty())
		{
			int Now = BuyQueue.front();
			if (1 <= Now && Now <= 8)
			{
				if (TryBuyItem(Now))
				{
					BuyQueue.pop();
					if (WantBuyIDs.count(Now))
					{
						WantBuyIDs.erase(Now);
					}
				}
				else
				{
					break;
				}
			}
		}
		*/

		if (!BuyQueue.empty())
		{
			int Now = BuyQueue.front();
			if (1 <= Now && Now <= 8)
			{
				TryBuyItem(Now);
			}
		}

		for (int i = 1; i <= 8; i += 1)
		{
			ItemSlost[i]->TextBuyOrder->SetText(FText::FromString(TEXT("")));
		}
		int Size = BuyQueue.size();
		for (int i = 1; i <= Size; i += 1)
		{
			int Now = BuyQueue.front();
			BuyQueue.pop();
			BuyQueue.push(Now);

			ItemSlost[Now]->TextBuyOrder->SetText(FText::FromString(FString::Printf(TEXT("%d"), i)));
		}
	}
}

void AHeroPlayerController::OnInputMoveForward(float Value)
{
	if (std::abs(Value) < 1e-4) return;

	auto SelfHero = Cast<AHeroCharacterBase>(GetPawn());
	if (SelfHero && SelfHero->AllowMove && !SelfHero->IsDead)
	{
		SelfHero->AddMovementInput(FVector(1, 0, 0), Value);
	}
}

void AHeroPlayerController::OnInputMoveRight(float Value)
{
	if (std::abs(Value) < 1e-4) return;

	auto SelfHero = Cast<AHeroCharacterBase>(GetPawn());
	if (SelfHero && SelfHero->AllowMove && !SelfHero->IsDead)
	{
		SelfHero->AddMovementInput(FVector(0, 1, 0), Value);
	}
}

void AHeroPlayerController::OnInputTab()
{
	ShowingTalent = !ShowingTalent;

	if (GameUI)
	{
		GameUI->OpenTalent(ShowingTalent);
	}
}

void AHeroPlayerController::OnInputSkillLMouse()
{
	auto Hero = Cast<AHeroCharacterBase>(GetPawn());
	if (GameUI && Hero && Hero->AllowSkill)
	{
		if (Hero->TargetingSkill)
		{
			auto HeroLocation = Hero->GetActorLocation();
			auto HeroTarget = GetMouseWorldLocation();

			FVector2D Location(HeroLocation.X, HeroLocation.Y);
			FVector2D Target(HeroTarget.X, HeroTarget.Y);

			switch (Hero->TargetingSkill)
			{
			case 1:
			{
				auto ManaCost = GI->SkillInfos[GameUI->Skill_LM->SaveID].ManaCost;
				if (GameUI->Skill_LM->Ready() && GameUI->ManaBar->Last() >= ManaCost)
				{
					Hero->SkillLM(Location, Target, true, false);
					// To Sync Skill
					/*
					if (Hero->SkillLM(Location, Target, true, false))
					{
						GameUI->Skill_LM->Active();
						Hero->SpendMana(ManaCost);
					}
					*/
				}
			}break;

			case 2:
			{
				auto ManaCost = GI->SkillInfos[GameUI->Skill_RM->SaveID].ManaCost;
				if (GameUI->Skill_RM->Ready() && GameUI->ManaBar->Last() >= ManaCost)
				{
					Hero->SkillRM(Location, Target, true, false);
					/*
					if (Hero->SkillRM(Location, Target, true, false))
					{
						GameUI->Skill_RM->Active();
						Hero->SpendMana(ManaCost);
					}
					*/
				}
			}break;

			case 3:
			{
				auto ManaCost = GI->SkillInfos[GameUI->Skill_Q->SaveID].ManaCost;
				if (GameUI->Skill_Q->Ready() && GameUI->ManaBar->Last() >= ManaCost)
				{
					Hero->SkillQ(Location, Target, true, false);
					/*
					if (Hero->SkillQ(Location, Target, true, false))
					{
						GameUI->Skill_Q->Active();
						Hero->SpendMana(ManaCost);
					}
					*/
				}
			}break;

			case 4:
			{
				auto ManaCost = GI->SkillInfos[GameUI->Skill_E->SaveID].ManaCost;
				if (GameUI->Skill_E->Ready() && GameUI->ManaBar->Last() >= ManaCost)
				{
					Hero->SkillE(Location, Target, true, false);
					/*
					if (Hero->SkillE(Location, Target, true, false))
					{
						GameUI->Skill_E->Active();
						Hero->SpendMana(ManaCost);
					}
					*/
				}
			}break;

			case 5:
			{
				auto ManaCost = GI->SkillInfos[GameUI->Skill_Space->SaveID].ManaCost;
				if (GameUI->Skill_Space->Ready() && GameUI->ManaBar->Last() >= ManaCost)
				{
					Hero->SkillSP(Location, Target, true, false);
					/*
					if (Hero->SkillSP(Location, Target, true, false))
					{
						GameUI->Skill_Space->Active();
						Hero->SpendMana(ManaCost);
					}
					*/
				}
			}break;
			}
		}
		else
		{
			if (!GI || !GI->SkillInfos.Contains(GameUI->Skill_LM->SaveID)) return;

			auto ManaCost = GI->SkillInfos[GameUI->Skill_LM->SaveID].ManaCost;
			if (GameUI->Skill_LM->Ready() && GameUI->ManaBar->Last() >= ManaCost)
			{
				auto HeroLocation = Hero->GetActorLocation();
				auto HeroTarget = GetMouseWorldLocation();

				FVector2D Location(HeroLocation.X, HeroLocation.Y);
				FVector2D Target(HeroTarget.X, HeroTarget.Y);

				Hero->SkillLM(Location, Target, false, false);
				/*
				if (Hero->SkillLM(Location, Target, false, false))
				{
					GameUI->Skill_LM->Active();
					Hero->SpendMana(ManaCost);
				}
				*/
			}
		}
	}
}

void AHeroPlayerController::OnInputSkillRMouse()
{
	auto Hero = Cast<AHeroCharacterBase>(GetPawn());
	if (GameUI && Hero && Hero->AllowSkill)
	{
		if (!GI || !GI->SkillInfos.Contains(GameUI->Skill_RM->SaveID)) return;

		auto ManaCost = GI->SkillInfos[GameUI->Skill_RM->SaveID].ManaCost;
		if (GameUI->Skill_RM->Ready() && GameUI->ManaBar->Last() >= ManaCost)
		{
			auto HeroLocation = Hero->GetActorLocation();
			auto HeroTarget = GetMouseWorldLocation();

			FVector2D Location(HeroLocation.X, HeroLocation.Y);
			FVector2D Target(HeroTarget.X, HeroTarget.Y);

			Hero->SkillRM(Location, Target, false, false);
			/*
			if (Hero->SkillRM(Location, Target, false, false))
			{
				GameUI->Skill_RM->Active();
				Hero->SpendMana(ManaCost);
			}
			*/
		}
	}
}

void AHeroPlayerController::OnInputSkillQ()
{
	auto Hero = Cast<AHeroCharacterBase>(GetPawn());
	if (GameUI && Hero && Hero->AllowSkill)
	{	
		if (!GI || !GI->SkillInfos.Contains(GameUI->Skill_Q->SaveID)) return;

		auto ManaCost = GI->SkillInfos[GameUI->Skill_Q->SaveID].ManaCost;
		if (GameUI->Skill_Q->Ready() && GameUI->ManaBar->Last() >= ManaCost)
		{
			auto HeroLocation = Hero->GetActorLocation();
			auto HeroTarget = GetMouseWorldLocation();

			FVector2D Location(HeroLocation.X, HeroLocation.Y);
			FVector2D Target(HeroTarget.X, HeroTarget.Y);

			Hero->SkillQ(Location, Target, false, false);
			/*
			if (Hero->SkillQ(Location, Target, false, false))
			{
				GameUI->Skill_Q->Active();
				Hero->SpendMana(ManaCost);
			}
			*/
		}
	}
}

void AHeroPlayerController::OnInputSkillE()
{
	auto Hero = Cast<AHeroCharacterBase>(GetPawn());
	if (GameUI && Hero && Hero->AllowSkill)
	{
		if (!GI || !GI->SkillInfos.Contains(GameUI->Skill_E->SaveID)) return;

		auto ManaCost = GI->SkillInfos[GameUI->Skill_E->SaveID].ManaCost;
		if (GameUI->Skill_E->Ready() && GameUI->ManaBar->Last() >= ManaCost)
		{
			auto HeroLocation = Hero->GetActorLocation();
			auto HeroTarget = GetMouseWorldLocation();

			FVector2D Location(HeroLocation.X, HeroLocation.Y);
			FVector2D Target(HeroTarget.X, HeroTarget.Y);

			Hero->SkillE(Location, Target, false, false);
			/*
			if (Hero->SkillE(Location, Target, false, false))
			{
				GameUI->Skill_E->Active();
				Hero->SpendMana(ManaCost);
			}
			*/
		}
	}
}

void AHeroPlayerController::OnInputSkillSpace()
{
	auto Hero = Cast<AHeroCharacterBase>(GetPawn());
	if (GameUI && Hero && Hero->AllowSkill)
	{
		if (!GI || !GI->SkillInfos.Contains(GameUI->Skill_Space->SaveID)) return;

		auto ManaCost = GI->SkillInfos[GameUI->Skill_Space->SaveID].ManaCost;
		if (GameUI->Skill_Space->Ready() && GameUI->ManaBar->Last() >= ManaCost)
		{
			auto HeroLocation = Hero->GetActorLocation();
			auto HeroTarget = GetMouseWorldLocation();

			FVector2D Location(HeroLocation.X, HeroLocation.Y);
			FVector2D Target(HeroTarget.X, HeroTarget.Y);

			Hero->SkillSP(Location, Target, false, false);
			/*
			if (Hero->SkillSP(Location, Target, false, false))
			{
				GameUI->Skill_Space->Active();
				Hero->SpendMana(ManaCost);
			}
			*/
		}
	}
}

void AHeroPlayerController::OnInputMenu()
{
	if (GameUI)
	{
		GameUI->OpenMenu();
	}
}

void AHeroPlayerController::OnPawnRecvItem(int ItemID)
{
	this->AttachItem(ItemID);

	USelfShopItemUserWidget* ShopItems[] =
	{
		nullptr,
		GameUI->Item_01, GameUI->Item_02, GameUI->Item_03, GameUI->Item_04,
		GameUI->Item_05, GameUI->Item_06, GameUI->Item_07, GameUI->Item_08
	};

	auto ItemPanelIndex = ItemID; // HARD CODE, may cause bug
	ShopItems[ItemPanelIndex]->Buy(false);

	auto SlotIndex = ItemPanelIndex;
	// Clear this item from wish list
	if (WantBuyIDs.count(SlotIndex))
	{
		WantBuyIDs.erase(SlotIndex);

		int Size = BuyQueue.size();
		for (int i = 1; i <= Size; i += 1)
		{
			int Index = BuyQueue.front();
			BuyQueue.pop();

			if (Index != SlotIndex)
			{
				BuyQueue.push(Index);
			}
		}
	}
}

void AHeroPlayerController::OnWantBuySome(int SlotIndex)
{
	if (WantBuyIDs.count(SlotIndex))
	{
		WantBuyIDs.erase(SlotIndex);

		int Size = BuyQueue.size();
		for (int i = 1; i <= Size; i += 1)
		{
			int Index = BuyQueue.front();
			BuyQueue.pop();

			if (Index != SlotIndex)
			{
				BuyQueue.push(Index);
			}
		}
	}
	else
	{
		WantBuyIDs.insert(SlotIndex);
		BuyQueue.push(SlotIndex);
	}
}

void AHeroPlayerController::BuyItem(int ItemPanelIndex)
{
	TryBuyItem(ItemPanelIndex);
}

bool AHeroPlayerController::TryBuyItem(int ItemPanelIndex)
{
	auto Hero = Cast<AHeroCharacterBase>(GetPawn());
	if (GameUI && Hero)
	{
		USelfShopItemUserWidget* ShopItems[] =
		{
			nullptr,
			GameUI->Item_01, GameUI->Item_02, GameUI->Item_03, GameUI->Item_04,
			GameUI->Item_05, GameUI->Item_06, GameUI->Item_07, GameUI->Item_08
		};

		int ItemID = ShopItems[ItemPanelIndex]->SaveID;
		int CostMonsy = ShopItems[ItemPanelIndex]->CostMoney;

		bool EnoughMoney = (Hero->MoneyNow >= CostMonsy);

		if (!ShopItems[ItemPanelIndex]->Bought && EnoughMoney && (LastBuyPassed > 1.0f))
		{
			if (!Hero->ClientStep.actionsinthisstep_size())
			{
				auto Action = Hero->ClientStep.add_actionsinthisstep();
				Action->set_type(-1 * ItemID);

				LastBuyPassed = 0.0f;
			}
		}
		/*
		bool Ret = Hero->SpendMoney(CostMonsy);
		if (Ret) // Can Buy
		{
			ShopItems[ItemPanelIndex]->Buy(false); // Really Buy
			// Apply Item
			Hero->AttachItem(ItemID); // First
			this->AttachItem(ItemID); // Second

			// Send a Message

			return true;
		}
		else // Not Enough Money
		{
			// Print(TEXT("Not Enough Money"));

			return false;
		}
		*/
	}

	return false;
}

void AHeroPlayerController::AttachItem(int ItemID)
{
	// Also in Hero Base

	auto Hero = Cast<AHeroCharacterBase>(GetPawn());
	if (GameUI && Hero)
	switch (ItemID)
	{
	case 1:
	{
		GameUI->UpdateHealth(Hero->HealthNow, Hero->HealthMax, Hero->HealthRecover);
		GameUI->HealthBar->ForceFlash();
	}break;

	case 2:
	{
		GameUI->UpdateHealth(Hero->HealthNow, Hero->HealthMax, Hero->HealthRecover);
		GameUI->HealthBar->ForceFlash();
	}break;

	case 3:
	{

	}break;

	case 4:
	{
		GameUI->UpdateMana(Hero->ManaNow, Hero->ManaMax, Hero->ManaRecover);
		GameUI->ManaBar->ForceFlash();
	}break;

	case 5:
	{
		GameUI->UpdateMana(Hero->ManaNow, Hero->ManaMax, Hero->ManaRecover);
		GameUI->ManaBar->ForceFlash();
	}break;

	case 6:
	{

	}break;

	case 7:
	{
		GameUI->Skill_LM->CooldownScale = 1.0f; // Different
		GameUI->Skill_RM->CooldownScale = 0.5f;
		GameUI->Skill_Q->CooldownScale = 0.5f;
		GameUI->Skill_E->CooldownScale = 0.5f;
		GameUI->Skill_Space->CooldownScale = 0.5f;
	}break;

	case 8:
	{

	}break;

	default:
	{
		Print(TEXT("Error Item ID!"));
	}break;
	}
}

void AHeroPlayerController::ShowWaitingUI()
{
	if (GameUI)
	{
		GameUI->GameRunning(false);
	}
}

void AHeroPlayerController::ReleaseSkill(int SkillID)
{
	auto SelfHero = Cast<AHeroCharacterBase>(GetPawn());
	if (!SelfHero || !GameUI || !GI) return;

	switch (SkillID)
	{
	case 1:
	{
		auto ManaCost = GI->SkillInfos[GameUI->Skill_LM->SaveID].ManaCost;
		GameUI->Skill_LM->Active();
		SelfHero->SpendMana(ManaCost);
	}break;

	case 2:
	{
		auto ManaCost = GI->SkillInfos[GameUI->Skill_RM->SaveID].ManaCost;
		GameUI->Skill_RM->Active();
		SelfHero->SpendMana(ManaCost);
	}break;

	case 3:
	{
		auto ManaCost = GI->SkillInfos[GameUI->Skill_Q->SaveID].ManaCost;
		GameUI->Skill_Q->Active();
		SelfHero->SpendMana(ManaCost);
	}break;

	case 4:
	{
		auto ManaCost = GI->SkillInfos[GameUI->Skill_E->SaveID].ManaCost;
		GameUI->Skill_E->Active();
		SelfHero->SpendMana(ManaCost);
	}break;

	case 5:
	{
		auto ManaCost = GI->SkillInfos[GameUI->Skill_Space->SaveID].ManaCost;
		GameUI->Skill_Space->Active();
		SelfHero->SpendMana(ManaCost);
	}break;

	default:
	{
		Print(TEXT("Wrong Skill ID"));
	}
	}
}

void AHeroPlayerController::AddGameUIToViewPort()
{
	if (GameUI && !AddedGameUIToViewPort)
	{
		GameUI->AddToViewport();

		AddedGameUIToViewPort = true;
	}
}

FVector AHeroPlayerController::GetMouseWorldLocation()
{
	auto PC = Cast<AHeroCharacterBase>(GetPawn());
	if (!PC) return FVector();

	return PC->GetMouseWorldLocation();
}
