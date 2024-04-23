// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	void CreateGameSession(int32 NumOfConnections, FName MatchType);
	void FindGameSession(int32 NumOfSearches);
	void JoinGameSession(const FOnlineSessionSearchResult& SearchResult);
	void StartGameSession();
	void DestroyGameSession();

protected:
	void OnCreateSessionCompleteDelegate(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsCompleteDelegate(bool bWasSuccessful);
	void OnJoinSessionCompleteDelegate(FName SessionName, EOnJoinSessionCompleteResult::Type SearchResult);
	void OnStartSessionCompleteDelegate(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionCompleteDelegate(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;

	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FDelegateHandle StartSessionCompleteDelegateHandle;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
};
