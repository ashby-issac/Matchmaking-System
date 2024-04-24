// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUserWidget.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"

void UMenuUserWidget::MenuSetup(int32 NumOfConnections, FString MatchType1)
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	if (auto World = GetWorld())
	{
		if (auto PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly UIInputMode;
			UIInputMode.SetWidgetToFocus(TakeWidget());
			UIInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(UIInputMode);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		NumOfPublicConnections = NumOfConnections;
		MatchType = MatchType1;
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
}

void UMenuUserWidget::OnHostButtonClicked()
{
	if (GEngine)
	{
		MultiplayerSessionsSubsystem->CreateGameSession(NumOfPublicConnections, MatchType);
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Black,
			FString::Printf(TEXT("Host Button Clicked"))
		);
	}
}

void UMenuUserWidget::OnJoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Black,
			FString::Printf(TEXT("Join Button Clicked"))
		);
	}
}

bool UMenuUserWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
	}

	return true;
}

void UMenuUserWidget::OnLevelRemovedFromWorld(ULevel* Level, UWorld* World)
{
	MenuTearDown();

	Super::OnLevelRemovedFromWorld(Level, World);
}

void UMenuUserWidget::MenuTearDown()
{
	RemoveFromParent();

	if (auto World = GetWorld())
	{
		if (auto PlayerController = World->GetFirstPlayerController())
		{
			FInputModeGameOnly GameInputMode;
			PlayerController->SetInputMode(GameInputMode);
		}
	}
}
