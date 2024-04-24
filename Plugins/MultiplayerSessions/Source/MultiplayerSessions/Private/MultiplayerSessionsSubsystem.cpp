// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleteDelegate)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsCompleteDelegate)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionCompleteDelegate)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionCompleteDelegate)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionCompleteDelegate))
{
	if (auto OnlineSubsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString::Printf(TEXT("Connected to steam: %s"), *OnlineSubsystem->GetSubsystemName().ToString())
			);
		}
	}
}

void UMultiplayerSessionsSubsystem::CreateGameSession(int32 NumOfConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bIsLANMatch = false;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->NumPublicConnections = NumOfConnections;
	SessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
}

void UMultiplayerSessionsSubsystem::FindGameSession(int32 NumOfSearches)
{
}

void UMultiplayerSessionsSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SearchResult)
{
}

void UMultiplayerSessionsSubsystem::StartGameSession()
{
}

void UMultiplayerSessionsSubsystem::DestroyGameSession()
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionCompleteDelegate(FName SessionName, bool bWasSuccessful)
{
	if (!GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString::Printf(TEXT("GEngine is null"))
		);
	}

	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Red,
					FString::Printf(TEXT("Successfully created session: %s"), *SessionName.ToString())
				);
			}
			World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString::Printf(TEXT("Failed to create session: %s"), *SessionName.ToString())
		);
	}
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsCompleteDelegate(bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnJoinSessionCompleteDelegate(FName SessionName, EOnJoinSessionCompleteResult::Type SearchResult)
{
}

void UMultiplayerSessionsSubsystem::OnStartSessionCompleteDelegate(FName SessionName, bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnDestroySessionCompleteDelegate(FName SessionName, bool bWasSuccessful)
{
}
