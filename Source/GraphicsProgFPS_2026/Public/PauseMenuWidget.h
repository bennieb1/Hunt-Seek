// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class GRAPHICSPROGFPS_2026_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// Must match button names in your WBP_PauseMenu
	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;

	// Set this in BP defaults (or hardcode if you want)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maps")
	FName MainMenuLevelName = TEXT("MainMenuLevel");

private:
	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnMainMenuClicked();

};
