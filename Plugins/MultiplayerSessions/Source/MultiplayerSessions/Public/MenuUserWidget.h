// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumOfConnections = 4, FString MatchType = "FreeForAll");

	UFUNCTION()
	void OnHostButtonClicked();
	
	UFUNCTION()
	void OnJoinButtonClicked();

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* Level, UWorld* World) override;

	UFUNCTION()
	void OnCreateSessionComplete(bool bWasSuccessful);

private:
	int32 NumOfPublicConnections;
	FString MatchType;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	void MenuTearDown();
};
