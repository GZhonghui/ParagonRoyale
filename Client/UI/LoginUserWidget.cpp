// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginUserWidget.h"

bool ULoginUserWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	if (LoginButton)
	{
		LoginButton->SetText(TEXT("Login"));
		LoginButton->PressedMe.AddUObject(this, &ULoginUserWidget::PushLogin);
	}
	if (SignupButton)
	{
		SignupButton->SetText(TEXT("Signup"));
		SignupButton->PressedMe.AddUObject(this, &ULoginUserWidget::PushSignup);
	}
	if (QuitButton)
	{
		QuitButton->SetText(TEXT("Quit"));
		QuitButton->PressedMe.AddUObject(this, &ULoginUserWidget::PushQuit);
		QuitButton->SetColor(FLinearColor(F(204), F(22), F(58), 1.0f));
	}
	if (ConfirmSignupButton)
	{
		ConfirmSignupButton->SetText(TEXT("Confirm"));
		ConfirmSignupButton->PressedMe.AddUObject(this, &ULoginUserWidget::PushConfirmSignup);
	}
	if (QuitSignupButton)
	{
		QuitSignupButton->SetText(TEXT("Close"));
		QuitSignupButton->PressedMe.AddUObject(this, &ULoginUserWidget::PushQuitSignup);
	}

	if (GI)
	{
		GI->OnLoginRespond.BindUObject(this, &ULoginUserWidget::LoginRespond);
		GI->OnSignupRespond.BindUObject(this, &ULoginUserWidget::SignupRespond);
	}

	return true;
}

void ULoginUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();
	if (WidgetClass->Animations.Num() >= 1)
	{
		PlayAnimation(WidgetClass->Animations[0]); // HARD CODE, Intro
	}
}

void ULoginUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (Enter)
	{
		WaitTime -= InDeltaTime;

		if (WaitTime < 0.0f)
		{
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/0_Main/Map/MainMap"));
		}
	}
}

void ULoginUserWidget::PushLogin()
{
	std::string UserNameStr = TCHAR_TO_UTF8(*UserName->GetText().ToString());
	std::string PasswordStr = TCHAR_TO_UTF8(*Password->GetText().ToString());

	if (UserNameStr.length() && PasswordStr.length() && GI &&
		UserNameStr.length() <= 32 && PasswordStr.length() <= 32)
	{
		PLoginRequest LoginRequest;
		LoginRequest.set_username(UserNameStr);
		LoginRequest.set_password(PasswordStr);

		TArray<uint8_t> Buffer;
		Buffer.SetNum(LoginRequest.ByteSizeLong());
		LoginRequest.SerializeToArray(Buffer.GetData(), LoginRequest.ByteSizeLong());

		GI->Send(MType::LoginRequest, Buffer.GetData(), LoginRequest.ByteSizeLong());

		GI->LoginUserName = UserNameStr; // Save the User Name
		LoginButton->SetIsEnabled(false);
	}
}

void ULoginUserWidget::PushSignup()
{
	UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();

	if (WidgetClass->Animations.Num() >= 2)
	{
		// Open Signup
		ConfirmSignupButton->SumTime = 0.0f;
		QuitSignupButton->SumTime = 0.0f;

		SignupUserName->SetText(FText::FromString(TEXT("")));
		SignupPassword_01->SetText(FText::FromString(TEXT("")));
		SignupPassword_02->SetText(FText::FromString(TEXT("")));
		SignupMessage->SetText(FText::FromString(TEXT("")));

		PlayAnimation(WidgetClass->Animations[1]); // HARD CODE
	}
}

void ULoginUserWidget::PushQuit()
{
	auto World = GetWorld();
	auto PC = UGameplayStatics::GetPlayerController(World, 0);
	
	UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, false);
}

void ULoginUserWidget::PushConfirmSignup()
{
	std::string UserNameStr = TCHAR_TO_UTF8(*SignupUserName->GetText().ToString());
	std::string PasswordStr_01 = TCHAR_TO_UTF8(*SignupPassword_01->GetText().ToString());
	std::string PasswordStr_02 = TCHAR_TO_UTF8(*SignupPassword_02->GetText().ToString());

	if (!UserNameStr.length())
	{
		SignupMessage->SetText(FText::FromString(TEXT("Input Username")));
		return;
	}
	if (UserNameStr.length() >= 12) // Username Length Limit
	{
		SignupMessage->SetText(FText::FromString(TEXT("Long Username")));
		return;
	}

	if (!PasswordStr_01.length())
	{
		SignupMessage->SetText(FText::FromString(TEXT("Input Password")));
		return;
	}
	if (PasswordStr_01.length() >= 32) // Password Length Limit
	{
		SignupMessage->SetText(FText::FromString(TEXT("Long Password")));
		return;
	}

	if (PasswordStr_01 != PasswordStr_02)
	{
		SignupMessage->SetText(FText::FromString(TEXT("Check Password")));
		return;
	}

	PRegisterRequest RegisterRequest;
	RegisterRequest.set_username(UserNameStr);
	RegisterRequest.set_password(PasswordStr_01);

	TArray<uint8_t> Buffer;
	Buffer.SetNum(RegisterRequest.ByteSizeLong());
	RegisterRequest.SerializeToArray(Buffer.GetData(), RegisterRequest.ByteSizeLong());

	GI->Send(MType::RegisterRequest, Buffer.GetData(), RegisterRequest.ByteSizeLong());

	SignupMessage->SetText(FText::FromString(TEXT("Waiting Server")));
	ConfirmSignupButton->SetIsEnabled(false);
}

void ULoginUserWidget::PushQuitSignup()
{
	UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();

	if (WidgetClass->Animations.Num() >= 3)
	{
		// Close Signup
		PlayAnimation(WidgetClass->Animations[2]); // HARD CODE
	}
}

// Login Successfully
void ULoginUserWidget::LoginRespond(int Successful, int ErrorCode)
{
	LoginButton->SetIsEnabled(true);
	if (Successful)
	{
		EnterMain->SetVisibility(ESlateVisibility::Visible);
		Enter = true;
	}
	else
	{
		ErrorUser->SetText(FText::FromString(TEXT("")));
		ErrorPass->SetText(FText::FromString(TEXT("")));
		if (ErrorCode == 1)
		{
			ErrorUser->SetText(FText::FromString(TEXT("User Error")));
		}
		else if (ErrorCode == 2)
		{
			ErrorPass->SetText(FText::FromString(TEXT("Pass Error")));
		}
		else if (ErrorCode == 3)
		{
			ErrorUser->SetText(FText::FromString(TEXT("Logined")));
		}
	}
}

void ULoginUserWidget::SignupRespond(int Successful, int ErrorCode)
{
	ConfirmSignupButton->SetIsEnabled(true);
	if (Successful)
	{
		SignupMessage->SetText(FText::FromString(TEXT("Signup Successful")));
	}
	else
	{
		SignupMessage->SetText(FText::FromString(TEXT("Same User Name")));
	}
}