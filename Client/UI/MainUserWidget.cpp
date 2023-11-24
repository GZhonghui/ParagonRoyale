// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUserWidget.h"

bool UMainUserWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	if (MatchButton)
	{
		MatchButton->SetText(TEXT("Match"));
		MatchButton->PressedMe.AddUObject(this, &UMainUserWidget::PushMatchButton);
	}
	if (SkillButton)
	{
		SkillButton->SetText(TEXT("About"));
		SkillButton->PressedMe.AddUObject(this, &UMainUserWidget::PushSkillButton);
	}
	if (QuitButton)
	{
		QuitButton->SetText(TEXT("Quit"));
		QuitButton->PressedMe.AddUObject(this, &UMainUserWidget::PushQuitButton);

		QuitButton->SetColor(FLinearColor(F(204), F(22), F(58), 1.0f));
	}
	if (CloseSkillButton)
	{
		CloseSkillButton->SetText(TEXT("Close"));
		CloseSkillButton->PressedMe.AddUObject(this, &UMainUserWidget::PushCloseSkillButton);
	}
	if (HeroPick_01)
	{
		HeroPick_01->SelfID = 1;
		if (GI)
		{
			HeroPick_01->SetImageFromPath(GI->HeroInfos[HeroPick_01->SelfID].MiniImagePath);
		}
		HeroPick_01->SelectedMe.AddUObject(this, &UMainUserWidget::HeroChange);
	}
	if (HeroPick_02)
	{
		HeroPick_02->SelfID = 2;
		if (GI)
		{
			HeroPick_02->SetImageFromPath(GI->HeroInfos[HeroPick_02->SelfID].MiniImagePath);
		}
		HeroPick_02->SelectedMe.AddUObject(this, &UMainUserWidget::HeroChange);
	}
	if (HeroPick_03)
	{
		HeroPick_03->SelfID = 3;
		if (GI)
		{
			HeroPick_03->SetImageFromPath(GI->HeroInfos[HeroPick_03->SelfID].MiniImagePath);
		}
		HeroPick_03->SelectedMe.AddUObject(this, &UMainUserWidget::HeroChange);
	}
	if (HeroPick_04)
	{
		HeroPick_04->SelfID = 4;
		if (GI)
		{
			HeroPick_04->SetImageFromPath(GI->HeroInfos[HeroPick_04->SelfID].MiniImagePath);
		}
		HeroPick_04->SelectedMe.AddUObject(this, &UMainUserWidget::HeroChange);
	}
	if (Skin_01)
	{
		Skin_01->SelfID = 1;
		Skin_01->SelectedMe.AddUObject(this, &UMainUserWidget::SkinChange);
	}
	if (Skin_02)
	{
		Skin_02->SelfID = 2;
		Skin_02->SelectedMe.AddUObject(this, &UMainUserWidget::SkinChange);
	}
	if (Skin_03)
	{
		Skin_03->SelfID = 3;
		Skin_03->SelectedMe.AddUObject(this, &UMainUserWidget::SkinChange);
	}
	if (Skin_04)
	{
		Skin_04->SelfID = 4;
		Skin_04->SelectedMe.AddUObject(this, &UMainUserWidget::SkinChange);
	}
	if (Skill_LM)
	{
		Skill_LM->SelfID = 1;
		Skill_LM->SelectedMe.AddUObject(this, &UMainUserWidget::SkillChange);
		Skill_LM->SetText(TEXT("L Mouse"));
	}
	if (Skill_RM)
	{
		Skill_RM->SelfID = 2;
		Skill_RM->SelectedMe.AddUObject(this, &UMainUserWidget::SkillChange);
		Skill_RM->SetText(TEXT("R Mouse"));
	}
	if (Skill_Q)
	{
		Skill_Q->SelfID = 3;
		Skill_Q->SelectedMe.AddUObject(this, &UMainUserWidget::SkillChange);
		Skill_Q->SetText(TEXT("Key Q"));
		Skill_Q->SetColor(FLinearColor(F(212), F(37), F(23), 1.0f));
	}
	if (Skill_E)
	{
		Skill_E->SelfID = 4;
		Skill_E->SelectedMe.AddUObject(this, &UMainUserWidget::SkillChange);
		Skill_E->SetText(TEXT("Key E"));
		Skill_E->SetColor(FLinearColor(F(212), F(37), F(23), 1.0f));
	}
	if (Skill_Space)
	{
		Skill_Space->SelfID = 5;
		Skill_Space->SelectedMe.AddUObject(this, &UMainUserWidget::SkillChange);
		Skill_Space->SetText(TEXT("Space"));
		Skill_Space->SetColor(FLinearColor(F(32), F(127), F(76), 1.0f));
	}

	HeroChange((int)FMath::FRandRange(1.1, 4.9));

	SkillPlayer = LoadObject<UMediaPlayer>(nullptr, TEXT("/Game/0_Main/Basic/Movie/Skills/SkillPlayer.SkillPlayer"));

	if (GI)
	{
		if (UserNameTextBlock)
		{
			UserNameTextBlock->SetText(FText::FromString(FString(TEXT("Welcome, ")) + FString(GI->LoginUserName.c_str())));
		}

		GI->OnMatchFound.BindUObject(this, &UMainUserWidget::MatchFound);
	}

	return true;
}

void UMainUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	WaitingTimeEnterGame -= InDeltaTime;
	if (WaitingTimeEnterGame > 0.0f) // Check Why should > 0.0f
	{
		if (WaitingToEnterGameTimer)
		{
			WaitingToEnterGameTimer->SetText(FText::FromString(FString::Printf(TEXT("Game will Begin in %d Seconds"), (int)WaitingTimeEnterGame)));
		}

		if (WaitingTimeEnterGame < 0.3f) // Enter Game
		{
			WaitingTimeEnterGame = -FMath::RandRange(3.0f, 7.0f);
		}
	}
	else if(WaitingTimeEnterGame > -100.0f)
	{
		if (LoadingPanel)
		{
			LoadingPanel->SetVisibility(ESlateVisibility::Visible);
		}

		if (WaitingTimeEnterGame < -10.0f)
		{
			// OpenLevel, Need GameMode?
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/0_Main/Map/GameMapStyle"));
		}
	}
}

void UMainUserWidget::HeroChange(int Which)
{
	if (Which <= 0 || Which >= 5 || Which == NowHero) // HARD CODE
	{
		return;
	}

	NowHero = Which;

	SkinChange(1);

	SpawnByHeroAndSkin(NowHero, NowSkin);

	USelfSelectableUserWidget* HeroSlots[] =
	{
		nullptr, HeroPick_01, HeroPick_02, HeroPick_03, HeroPick_04
	};
	FString HeroNames[] =
	{
		TEXT(""), TEXT("Aurora"), TEXT("FengMao"), TEXT("Grux"), TEXT("Sparrow")
	};
	int SkinNumber[] = { 0,4,3,4,4 };

	for (int i = 1; i <= 4; i += 1)
	{
		HeroSlots[i]->Selected = (HeroSlots[i]->SelfID == NowHero);
	}
	HeroName->SetText(FText::FromString(HeroNames[NowHero]));
	SetSkinNumber(SkinNumber[NowHero]);

	switch (NowHero)
	{
	case 1:
	{
		Skin_01->SetColor(FLinearColor(F( 54), F( 41), F( 47), 1.0f));
		Skin_02->SetColor(FLinearColor(F(138), F(188), F(209), 1.0f));
		Skin_03->SetColor(FLinearColor(F(126), F( 22), F(113), 1.0f));
		Skin_04->SetColor(FLinearColor(F( 26), F(148), F(188), 1.0f));
	}break;
	case 2:
	{
		Skin_01->SetColor(FLinearColor(F(254), F(204), F( 17), 1.0f));
		Skin_02->SetColor(FLinearColor(F( 16), F(174), F(194), 1.0f));
		Skin_03->SetColor(FLinearColor(F( 69), F(183), F(135), 1.0f));
	}break;
	case 3:
	{
		Skin_01->SetColor(FLinearColor(F(240), F( 63), F( 36), 1.0f));
		Skin_02->SetColor(FLinearColor(F(241), F(151), F(144), 1.0f));
		Skin_03->SetColor(FLinearColor(F(147), F(213), F(220), 1.0f));
		Skin_04->SetColor(FLinearColor(F(196), F(203), F(207), 1.0f));
	}break;
	case 4:
	{
		Skin_01->SetColor(FLinearColor(F(122), F(115), F(116), 1.0f));
		Skin_02->SetColor(FLinearColor(F( 78), F( 42), F( 64), 1.0f));
		Skin_03->SetColor(FLinearColor(F(209), F(194), F(211), 1.0f));
		Skin_04->SetColor(FLinearColor(F(207), F( 72), F( 19), 1.0f));
	}break;
	}
}

void UMainUserWidget::SkinChange(int Which)
{
	if (Which <= 0 || Which >= 5 || Which == NowSkin) // HARD CODE
	{
		return;
	}

	USelfSelectableUserWidget* SkinSlots[] =
	{
		nullptr, Skin_01, Skin_02, Skin_03, Skin_04
	};

	for (int i = 1; i <= 4; i += 1)
	{
		SkinSlots[i]->Selected = (SkinSlots[i]->SelfID == Which);
	}

	NowSkin = Which;

	SpawnByHeroAndSkin(NowHero, NowSkin);
}

void UMainUserWidget::SkillChange(int Which)
{
	if (Which <= 0 || Which >= 6 || Which == NowSkill) // HARD CODE
	{
		return;
	}

	NowSkill = Which;

	USelfSelectableUserWidget* SkillSlots[] =
	{
		nullptr, Skill_LM, Skill_RM, Skill_Q, Skill_E, Skill_Space
	};

	for (int i = 1; i <= 5; i += 1)
	{
		SkillSlots[i]->Selected = (NowSkill == i);
	}

	PlayVideoByHeroAndSkill(NowHero, NowSkill);
}

void UMainUserWidget::SetSkinNumber(int Number)
{
	USelfSelectableUserWidget* SkinSlots[] =
	{
		nullptr, Skin_01, Skin_02, Skin_03, Skin_04
	};

	for (int i = 1; i <= 4; i += 1)
	{
		if (i <= Number)
		{
			SkinSlots[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			SkinSlots[i]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UMainUserWidget::SpawnByHeroAndSkin(int Hero, int Skin)
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	TArray<AActor*> PreviewActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("HeroPreview")), PreviewActors);
	if (!PreviewActors.Num()) return;
	auto PreviewTransform = PreviewActors[0]->GetTransform();

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	switch (Hero)
	{
	case 1:
	{
		if(PreviewActorAuroraClasses.Num() >= 4) // HARD CODE
		PreviewActor = GetWorld()->SpawnActor<AHeroPreviewActor>(
			PreviewActorAuroraClasses[Skin - 1].Get(), PreviewTransform, ActorSpawnParams);
	}break;

	case 2:
	{
		if (PreviewActorFengMaoClasses.Num() >= 3) // HARD CODE
		PreviewActor = GetWorld()->SpawnActor<AHeroPreviewActor>(
			PreviewActorFengMaoClasses[Skin - 1].Get(), PreviewTransform, ActorSpawnParams);
	}break;

	case 3:
	{
		if (PreviewActorGruxClasses.Num() >= 4) // HARD CODE
		PreviewActor = GetWorld()->SpawnActor<AHeroPreviewActor>(
			PreviewActorGruxClasses[Skin - 1].Get(), PreviewTransform, ActorSpawnParams);
	}break;

	case 4:
	{
		if (PreviewActorSparrowClasses.Num() >= 4) // HARD CODE
		PreviewActor = GetWorld()->SpawnActor<AHeroPreviewActor>(
			PreviewActorSparrowClasses[Skin - 1].Get(), PreviewTransform, ActorSpawnParams);
	}break;
	}
}

void UMainUserWidget::PlayVideoByHeroAndSkill(int Hero, int Skill)
{
	if (!SkillPlayer) return;

	FString VideoPath[5][6] = // HARD CODE 4 * 5
	{
		{},
		{
			TEXT(""),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Aurora_Skill_01"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Aurora_Skill_02"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Aurora_Skill_03"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Aurora_Skill_04"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Aurora_Skill_05")
		},
		{
			TEXT(""),
			TEXT("/Game/0_Main/Basic/Movie/Skills/FengMao_Skill_01"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/FengMao_Skill_02"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/FengMao_Skill_03"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/FengMao_Skill_04"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/FengMao_Skill_05")
		},
		{
			TEXT(""),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Grux_Skill_01"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Grux_Skill_02"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Grux_Skill_03"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Grux_Skill_04"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Grux_Skill_05")
		},
		{
			TEXT(""),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Sparrow_Skill_01"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Sparrow_Skill_02"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Sparrow_Skill_03"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Sparrow_Skill_04"),
			TEXT("/Game/0_Main/Basic/Movie/Skills/Sparrow_Skill_05")
		}
	};

	UMediaSource* VideoSource = LoadObject<UMediaSource>(nullptr, *VideoPath[Hero][Skill]);
	if (VideoSource)
	{
		SkillPlayer->OpenSource(VideoSource);
		SkillPlayer->SetLooping(true);
	}
}

void UMainUserWidget::PushSkillButton()
{
	UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();

	if (WidgetClass->Animations.Num() >= 1)
	{
		// Open Skill
		CloseSkillButton->SumTime = 0.0f;
		PlayAnimation(WidgetClass->Animations[0]); // HARD CODE

		SkillChange(1);
		PlayVideoByHeroAndSkill(NowHero, 1);
	}
}

void UMainUserWidget::PushMatchButton()
{
	USelfSelectableUserWidget* SkinSlots[] =
	{
		nullptr, Skin_01, Skin_02, Skin_03, Skin_04
	};
	USelfSelectableUserWidget* HeroSlots[] =
	{
		nullptr, HeroPick_01, HeroPick_02, HeroPick_03, HeroPick_04
	};

	IsMatching = !IsMatching;
	if (IsMatching)
	{
		MatchButton->SetText(TEXT("MATCHING"));
		MatchingThrobber->SetVisibility(ESlateVisibility::Visible);

		PMatchRequest MatchRequest;
		MatchRequest.set_heroid(NowHero);
		MatchRequest.set_skinid(NowSkin);
		MatchRequest.set_username(GI->LoginUserName);

		TArray<uint8_t> Buffer;
		Buffer.SetNum(MatchRequest.ByteSizeLong());
		MatchRequest.SerializeToArray(Buffer.GetData(), MatchRequest.ByteSizeLong());

		GI->Send(MType::MatchRequest, Buffer.GetData(), MatchRequest.ByteSizeLong());
	}
	else
	{
		MatchButton->SetText(TEXT("MATCH"));
		MatchingThrobber->SetVisibility(ESlateVisibility::Hidden);

		GI->Send(MType::CancelMatchRequest, nullptr, 0);
	}

	for (int i = 1; i <= 4; i += 1)
	{
		HeroSlots[i]->SetIsEnabled(!IsMatching);
		SkinSlots[i]->SetIsEnabled(!IsMatching);
	}
}

void UMainUserWidget::PushQuitButton()
{
	auto World = GetWorld();
	auto PC = UGameplayStatics::GetPlayerController(World, 0);

	UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, false);
}

void UMainUserWidget::PushCloseSkillButton()
{
	UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();

	if (WidgetClass->Animations.Num() >= 2)
	{
		// Close Skill
		PlayAnimation(WidgetClass->Animations[1]); // HARD CODE
	}
}

void UMainUserWidget::MatchFound()
{
	if (MatchFoundCanvas)
	{
		MatchFoundCanvas->SetVisibility(ESlateVisibility::Visible);
		WaitingTimeEnterGame = 5.0f;
	}

	if (GI)
	{
		USelfLoadingHeroSlotUserWidget* HeroSlots[] =
		{
			LoadingHero_01, LoadingHero_02, LoadingHero_03, LoadingHero_04
		}; // Start From 0
		const PMatchFoundRespond& PlayersInfo = GI->MatchFoundRespondResult;
		for (int i = 0; i < std::min(4, PlayersInfo.otherplayers_size()); i += 1)
		{
			FString ThisPlayerName(PlayersInfo.otherplayers(i).playername().c_str());
			HeroSlots[i]->SetHeroAndUsername(PlayersInfo.otherplayers(i).heroid(), ThisPlayerName);
		}

		int MapID = GI->MapSeed % (GI->GameMapNumber + 1);
		if (MapID)
		{
			// TextMapName->SetText(FText::FromString(FString::Printf(TEXT("Map Name: %s"), GI->MapName[MapID].c_str())));
			TextMapName->SetText(FText::FromString(FString(TEXT("Map Name: ")) + FString(GI->MapName[MapID].c_str())));
		}
		else
		{
			TextMapName->SetText(FText::FromString(TEXT("Map Name: Random Royal")));
		}
	}
}