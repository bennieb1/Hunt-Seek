// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.h"


void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnMainMenuClicked);
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	// Best: call your custom controller function (restores input mode/cursor)
	if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetOwningPlayer()))
	{
		PC->ResumeFromPause();
		return;
	}

	// Fallback if you're not using a custom controller yet:
	if (APlayerController* PC2 = GetOwningPlayer())
	{
		UGameplayStatics::SetGamePaused(this, false);

		bIsFocusable = false;
		RemoveFromParent();

		PC2->bShowMouseCursor = false;
		FInputModeGameOnly Mode;
		PC2->SetInputMode(Mode);
	}
}

void UPauseMenuWidget::OnMainMenuClicked()
{
	// Always unpause before changing level
	UGameplayStatics::SetGamePaused(this, false);

	UGameplayStatics::OpenLevel(this, MainMenuLevelName);

}