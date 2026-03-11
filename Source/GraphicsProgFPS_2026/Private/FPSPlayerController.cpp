#include "FPSPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../FPSCharacter.h"
#include "HudMenu.h"
#include "GameOverMenu.h"

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const FString MapName = GetWorld() ? GetWorld()->GetMapName() : FString();

	// Add gameplay context by default (so Pause works in gameplay)
	EnableGameplayContext();

	// If we're on a menu map, show main menu; otherwise show HUD
	if (MapName.Contains(MenuMapNameContains))
	{
		ShowMainMenu();
	}
	else
	{
		ShowHUD();
	}
}

void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (PauseAction)
		{
			EIC->BindAction(PauseAction, ETriggerEvent::Started, this, &AFPSPlayerController::TogglePauseMenu);
		}
	}
}

void AFPSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckPlayerDeath();

}

void AFPSPlayerController::ShowMainMenu()
{
	if (!MainMenuClass) return;

	if (!MainMenuWidget)
	{
		MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuClass);
	}

	if (MainMenuWidget && !MainMenuWidget->IsInViewport())
	{
		MainMenuWidget->AddToViewport(100);
	}

	bMainMenuOpen = true;

	EnableUIContext();
	SetUIOnly(MainMenuWidget);
}

void AFPSPlayerController::HideMainMenu()
{
	if (MainMenuWidget && MainMenuWidget->IsInViewport())
	{
		MainMenuWidget->RemoveFromParent();
	}

	bMainMenuOpen = false;

	EnableGameplayContext();
	SetGameOnly();
}

void AFPSPlayerController::ShowHUD()
{
	if (!HudClass) return;

	if (!HudWidget)
	{
		// ✅ Proper typed CreateWidget call
		HudWidget = CreateWidget<UHudMenu>(this, HudClass);
	}

	if (HudWidget && !HudWidget->IsInViewport())
	{
		HudWidget->AddToViewport(0);
	}

	// init HUD values
	HudWidget->SetScore(CurrentScore);

	HudStartTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			HudTimeTimerHandle,
			this,
			&AFPSPlayerController::UpdateHudTime,
			0.1f,
			true
		);
	}
}

void AFPSPlayerController::HideHUD()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HudTimeTimerHandle);
	}

	if (HudWidget && HudWidget->IsInViewport())
	{
		HudWidget->RemoveFromParent();
	}
}

void AFPSPlayerController::UpdateHudTime()
{
	if (!HudWidget || !GetWorld()) return;

	const float Elapsed = GetWorld()->GetTimeSeconds() - HudStartTimeSeconds;
	HudWidget->SetElapsedTime(Elapsed);
}

void AFPSPlayerController::CheckPlayerDeath()
{

	if (bGameOverShown) return;
	if (UGameplayStatics::IsGamePaused(this)) return;

	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(GetPawn());
	if (!FPSCharacter) return;

	if (FPSCharacter->GetCurrentHealth() <= 0.f)
	{
		const float FinalTime = GetElapsedHudTimeSeconds();
		ShowGameOver(FinalTime, false);
	}

}

void AFPSPlayerController::AddScore(int32 Amount)
{
	CurrentScore += Amount;

	if (HudWidget)
	{
		HudWidget->SetScore(CurrentScore);
	}
}

void AFPSPlayerController::SetHUDHealth(float Current, float Max)
{
	if (HudWidget)
	{
		HudWidget->SetHealth(Current, Max);
	}
}

float AFPSPlayerController::GetElapsedHudTimeSeconds() const
{
	if (!GetWorld()) return 0.f;
	return GetWorld()->GetTimeSeconds() - HudStartTimeSeconds;
}

void AFPSPlayerController::TogglePauseMenu()
{
	const bool bPaused = UGameplayStatics::IsGamePaused(this);

	if (!bPaused)
	{
		UGameplayStatics::SetGamePaused(this, true);

		if (!PauseMenuClass) return;

		if (!PauseWidget)
		{
			PauseWidget = CreateWidget<UUserWidget>(this, PauseMenuClass);
		}

		if (PauseWidget && !PauseWidget->IsInViewport())
		{
			PauseWidget->AddToViewport(20);
		}

		bPauseMenuOpen = true;

		EnableUIContext();
		SetUIOnly(PauseWidget);
	}
	else
	{
		ResumeFromPause();
	}
}

void AFPSPlayerController::ResumeFromPause()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (PauseWidget && PauseWidget->IsInViewport())
	{
		PauseWidget->RemoveFromParent();
	}

	bPauseMenuOpen = false;

	EnableGameplayContext();
	SetGameOnly();
}

void AFPSPlayerController::ShowGameOver(float FinalTimeSeconds, bool bPlayerWon)
{
	if (bGameOverShown) return;
	bGameOverShown = true;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HudTimeTimerHandle);
	}

	UGameplayStatics::SetGamePaused(this, true);

	if (GameOverClass)
	{
		if (!GameOverWidget)
		{
			GameOverWidget = CreateWidget<UGameOverMenu>(this, GameOverClass);
		}

		if (GameOverWidget && !GameOverWidget->IsInViewport())
		{
			GameOverWidget->AddToViewport(50);
		}

		if (GameOverWidget)
		{
			GameOverWidget->SetFinalTimeSeconds(FinalTimeSeconds);
			GameOverWidget->SetResultMessage(bPlayerWon ? TEXT("You Win") : TEXT("You Lose"));
		}
	}

	EnableUIContext();
	SetUIOnly(GameOverWidget);
}

void AFPSPlayerController::SetUIOnly(UUserWidget* FocusWidget)
{
	bShowMouseCursor = true;

	FInputModeUIOnly Mode;
	if (FocusWidget)
	{
		Mode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(Mode);
}

void AFPSPlayerController::SetGameOnly()
{
	bShowMouseCursor = false;

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
}

void AFPSPlayerController::EnableUIContext()
{
	if (!UIContext) return;

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(UIContext, 10);
		}
	}
}

void AFPSPlayerController::EnableGameplayContext()
{
	if (!GameplayContext) return;

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(GameplayContext, 0);
		}
	}
}