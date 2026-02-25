// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class GRAPHICSPROGFPS_2026_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

    virtual void NativeConstruct() override;

    // These MUST match button names in WBP
    UPROPERTY(meta = (BindWidget))
    UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    UButton* QuitButton;

private:

    UFUNCTION()
    void OnPlayClicked();

    UFUNCTION()
    void OnQuitClicked();
	
};
