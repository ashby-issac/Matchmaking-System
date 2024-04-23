// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

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

void UMultiplayerSessionsSubsystem::CreateGameSession(int32 NumOfConnections, FName MatchType)
{
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
