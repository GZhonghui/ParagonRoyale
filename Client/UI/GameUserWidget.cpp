// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUserWidget.h"

bool UGameUserWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	if (Skill_Tab)
	{
		Skill_Tab->SetText(TEXT("TAB"));
		Skill_Tab->SetCenterImageFromPath(TEXT("/Game/0_Main/Basic/Icon/Skill/Shop")); // HARD CODE
	}
	if (Skill_LM)
	{
		Skill_LM->SetText(TEXT("LM"));
		Skill_LM->SetSkillID(1); // HARD CODE
	}
	if (Skill_RM)
	{
		Skill_RM->SetText(TEXT("RM"));
		Skill_RM->SetSkillID(2); // HARD CODE
	}
	if (Skill_Q)
	{
		Skill_Q->SetText(TEXT("Q"));
		Skill_Q->SetSkillID(3); // HARD CODE
	}
	if (Skill_E)
	{
		Skill_E->SetText(TEXT("E"));
		Skill_E->SetSkillID(4); // HARD CODE
	}
	if (Skill_Space)
	{
		Skill_Space->SetText(TEXT("Space"));
		Skill_Space->SetSkillID(5); // HARD CODE
	}
	if (Item_01)
	{
		Item_01->SelfID = 1; // Slot Index
		Item_01->SetItemID(1); // Item ID
	}
	if (Item_02)
	{
		Item_02->SelfID = 2;
		Item_02->SetItemID(2);
	}
	if (Item_03)
	{
		Item_03->SelfID = 3;
		Item_03->SetItemID(3);
	}
	if (Item_04)
	{
		Item_04->SelfID = 4;
		Item_04->SetItemID(4);
	}
	if (Item_05)
	{
		Item_05->SelfID = 5;
		Item_05->SetItemID(5);
	}
	if (Item_06)
	{
		Item_06->SelfID = 6;
		Item_06->SetItemID(6);
	}
	if (Item_07)
	{
		Item_07->SelfID = 7;
		Item_07->SetItemID(7);
	}
	if (Item_08)
	{
		Item_08->SelfID = 8;
		Item_08->SetItemID(8);
	}

	if (SingCanvasPanel)
	{
		SingCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	}

	if (MenuResume)
	{
		MenuResume->SetText(TEXT("Resume"));
		MenuResume->PressedMe.AddUObject(this, &UGameUserWidget::OnPushResume);
	}
	if (MenuQuit)
	{
		MenuQuit->SetText(TEXT("Quit"));
		MenuQuit->PressedMe.AddUObject(this, &UGameUserWidget::OnPushQuit);
	}

	if (ButtonVoteYes)
	{
		ButtonVoteYes->SetText(TEXT("Yes"));
		ButtonVoteYes->PressedMe.AddUObject(this, &UGameUserWidget::OnPushVote);
	}
	if (ButtonVoteNo)
	{
		ButtonVoteNo->SetText(TEXT("No"));
		ButtonVoteNo->PressedMe.AddUObject(this, &UGameUserWidget::OnPushVote);
	}
	if (ButtonResultQuit)
	{
		ButtonResultQuit->SetText(TEXT("Quit"));
		ButtonResultQuit->PressedMe.AddUObject(this, &UGameUserWidget::OnPushQuit);
	}

	return true;
}

void UGameUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Need Boost

	USelfSkillUserWidget* SkillSlots[] =
	{
		nullptr, Skill_LM, Skill_RM, Skill_Q, Skill_E, Skill_Space
	};

	// Update: Is Mana Enough for every skill
	for (int i = 1; i <= 5; i += 1)
	{
		if (GI)
		{
			const TMap<int, FSkillInfo>& LocalSkillInfos = GI->SkillInfos;
			if (LocalSkillInfos.Contains(SkillSlots[i]->SaveID))
			{
				auto ManaCost = LocalSkillInfos[SkillSlots[i]->SaveID].ManaCost;
				SkillSlots[i]->SetIsEnabled(ManaBar->Last() >= ManaCost);
			}
		}
	}

	UpdateInfo();

	DiePanelTime += InDeltaTime;
	if (ShowingDiePanel && DiePanelTime > 4.0f)
	{
		ShowingDiePanel = false;

		UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();

		if (WidgetClass->Animations.Num() >= 4)
		{
			PlayAnimation(WidgetClass->Animations[3]); // HARD CODE, CloseDie
		}
	}

	if (WillQuitGame)
	{
		if (BackPanel)
		{
			BackPanel->SetVisibility(ESlateVisibility::Visible);
		}
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/0_Main/Map/MainMap"));
	}
}

// Set Skills from Skill ID
void UGameUserWidget::SetSkills(const int* Skills)
{
	if (!Skills) return;

	if (Skill_LM)
	{
		Skill_LM->SetSkillID(Skills[1]);
	}
	if (Skill_RM)
	{
		Skill_RM->SetSkillID(Skills[2]);
	}
	if (Skill_Q)
	{
		Skill_Q->SetSkillID(Skills[3]);
	}
	if (Skill_E)
	{
		Skill_E->SetSkillID(Skills[4]);
	}
	if (Skill_Space)
	{
		Skill_Space->SetSkillID(Skills[5]);
	}
}

void UGameUserWidget::UpdateHealth(float Now, float Max, float Recover)
{
	HealthBar->SetMax(Max); // Set Max First
	HealthBar->SetNow(Now);

	HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f / %.1f"), Now, Max, Recover)));
}

void UGameUserWidget::UpdateMana(float Now, float Max, float Recover)
{
	ManaBar->SetMax(Max);
	ManaBar->SetNow(Now);

	ManaText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f / %.1f"), Now, Max, Recover)));
}

void UGameUserWidget::UpdateMoney(int Money)
{
	LastMoney = Money;

	MoneyText->SetText(FText::FromString(FString::Printf(TEXT("Money : %d"), LastMoney)));
}

void UGameUserWidget::UpdateSingState(float Last, float Total)
{
	if (SingCanvasPanel)
	{
		if (Last < 1e-4)
		{
			SingCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			SingCanvasPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
			if (SingProgressBar)
			{
				SingProgressBar->SetPercent(Last / Total);
			}
			if (SingTextBlock)
			{
				SingTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Singing : %.1f"), Last)));
			}
		}
	}

}

void UGameUserWidget::UpdateInfo()
{
	static int RunTime = 0;
	RunTime += 1;

	if (GI)
	{
		auto LGI = GI;
		if (InfoGameTime)
		{
			InfoGameTime->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1lfS"), LGI->GetGameTime())));
		}
		if (InfoStepCount)
		{
			InfoStepCount->SetText(FText::FromString(FString::Printf(TEXT("Step: %d"), LGI->GetStep())));
		}
		if (InfoDelay)
		{
			if (!(RunTime % 10))
			{
				InfoDelay->SetText(FText::FromString(FString::Printf(TEXT("Delay: %.1lfMS"), LGI->GetDelay())));
			}
		}
		if (InfoCheck)
		{
			InfoCheck->SetText(FText::FromString(FString::Printf(TEXT("Check: %.1lf%%"), 100 * LGI->GetCheck())));
		}
		if (InfoLost)
		{
			InfoLost->SetText(FText::FromString(FString::Printf(TEXT("Lost: %d"), LGI->GetLost())));
		}
		if (InfoLastPlayerNumber)
		{
			InfoLastPlayerNumber->SetText(FText::FromString(FString::Printf(TEXT("Player: %d / %d"), LGI->GetPlayerNumber(), LGI->RoomSize)));
		}
	}
}

void UGameUserWidget::OnPushResume()
{
	if (MenuPanel)
	{
		MenuPanel->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGameUserWidget::OnPushQuit()
{
	if (ButtonResultQuit)
	{
		ButtonResultQuit->SetIsEnabled(false);
	}
	if (MenuQuit)
	{
		MenuQuit->SetIsEnabled(false);
	}
	if (GI)
	{
		GI->GameStop();
		GI->Send(MType::QuitRequest, nullptr, 0);
		WillQuitGame = true;
	}
}

void UGameUserWidget::OnPushVote()
{
	if (ButtonVoteYes)
	{
		ButtonVoteYes->SetIsEnabled(false);
	}
	if (ButtonVoteNo)
	{
		ButtonVoteNo->SetIsEnabled(false);
	}
	if (VoteText)
	{
		VoteText->SetText(FText::FromString(TEXT("Thank you for your play!")));
	}
}

void UGameUserWidget::OpenTalent(bool State)
{
	UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();

	if (WidgetClass->Animations.Num() >= 2)
	{
		if (State)
		{
			PlayAnimation(WidgetClass->Animations[0]); // HARD CODE, OpenTalent
		}
		else
		{
			PlayAnimation(WidgetClass->Animations[1]); // HARD CODE, CloseTalent
		}
	}
}

void UGameUserWidget::OpenMenu()
{
	if (MenuPanel)
	{
		if (MenuPanel->GetVisibility() == ESlateVisibility::Hidden)
		{
			MenuPanel->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			MenuPanel->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UGameUserWidget::ShowPlayerDie(FString UserName)
{
	if (TextPlayerDied)
	{
		TextPlayerDied->SetText(FText::FromString(UserName + TEXT(" Died!")));
	}

	UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();

	if (WidgetClass->Animations.Num() >= 3)
	{
		PlayAnimation(WidgetClass->Animations[2]); // HARD CODE, OpenDie
	}

	DiePanelTime = 0.0f;
	ShowingDiePanel = true;
}

void UGameUserWidget::GameRunning(bool Running)
{
	if (Running)
	{
		if (MainPanel) MainPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (WaitingPlayerPanel) WaitingPlayerPanel->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		if (WaitingPlayerPanel) WaitingPlayerPanel->SetVisibility(ESlateVisibility::Visible);
	}
}