// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

#include "SelfButtonUserWidget.h"

#include "../Instance/MainGameInstance.h"

#include "LoginUserWidget.generated.h"

UCLASS()
class PARAGONROYAL_API ULoginUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	bool Enter = false; // is Login Successfull
	float WaitTime = 3.0f; // Whit 3 Seconds to Load MainMap

public:
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* UserName;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Password;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* LoginButton;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* SignupButton;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* SignupUserName;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* SignupPassword_01;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* SignupPassword_02;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ErrorUser;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ErrorPass;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* ConfirmSignupButton;

	UPROPERTY(meta = (BindWidget))
	USelfButtonUserWidget* QuitSignupButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SignupMessage;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* EnterMain;

public:
	void PushLogin();
	void PushSignup();
	void PushQuit();
	void PushConfirmSignup();
	void PushQuitSignup();

// Message From Server
public:
	UFUNCTION()
	void LoginRespond(int Successful, int ErrorCode); // For Login

	UFUNCTION()
	void SignupRespond(int Successful, int ErrorCode); // For Signup
};
