// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class UUserWidget;
class UInputAction;
class UInputMappingContext;
class UHudMenu;
class UGameOverMenu;

/**
 * 
 */
UCLASS()
class GRAPHICSPROGFPS_2026_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// ---------- UI Classes (assign in BP defaults) ----------
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuClass;

	// IMPORTANT: WBP_HUD must have Parent Class = HudMenu (UHudMenu)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHudMenu> HudClass;

	// IMPORTANT: WBP_GameOver must have Parent Class = GameOverMenu (UGameOverMenu)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameOverMenu> GameOverClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	// ---------- Enhanced Input ----------
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> GameplayContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> UIContext;

	// ---------- Map Names ----------
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FString MenuMapNameContains = TEXT("MainMenu");

	// ---------- UI Controls ----------
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ResumeFromPause();

	// ---------- HUD API ----------
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetHUDHealth(float Current, float Max);

	float GetElapsedHudTimeSeconds() const;

	// ---------- Game Over ----------
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameOver(float FinalTimeSeconds);

private:
	// Widget instances (ONE of each)
	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenuWidget;

	UPROPERTY()
	TObjectPtr<UHudMenu> HudWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> PauseWidget;

	UPROPERTY()
	TObjectPtr<UGameOverMenu> GameOverWidget;

	// State
	bool bMainMenuOpen = false;
	bool bPauseMenuOpen = false;

	// Score/time
	int32 CurrentScore = 0;
	float HudStartTimeSeconds = 0.f;
	FTimerHandle HudTimeTimerHandle;

	// Helpers
	void UpdateHudTime();

	void SetUIOnly(UUserWidget* FocusWidget);
	void SetGameOnly();

	void EnableUIContext();
	void EnableGameplayContext();

};
