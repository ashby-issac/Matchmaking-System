// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
	if (!PlayerState)
	{
		return;
	}

	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			15.f,
			FColor::Cyan,
			FString::Printf(TEXT("Number of players: %d"), NumOfPlayers)
		);

		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has joined the game"), *PlayerState->GetPlayerName())
		);
	}
}

void ALobbyGameMode::Logout(AController* Player)
{
	Super::Logout(Player);

	APlayerState* PlayerState = Player->GetPlayerState<APlayerState>();
	if (!PlayerState)
	{
		return;
	}

	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			15.f,
			FColor::Cyan,
			FString::Printf(TEXT("Number of players: %d"), NumOfPlayers)
		);

		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has joined the game"), *PlayerState->GetPlayerName())
		);
	}
}


