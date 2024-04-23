// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUserWidget.h"

void UMenuUserWidget::MenuSetup()
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
}
