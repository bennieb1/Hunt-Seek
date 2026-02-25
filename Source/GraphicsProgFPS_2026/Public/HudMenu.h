// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudMenu.generated.h"

class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS()
class GRAPHICSPROGFPS_2026_API UHudMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void SetScore(int32 NewScore);
	UFUNCTION(BlueprintCallable) void SetHealth(float Current, float Max);
	UFUNCTION(BlueprintCallable) void SetElapsedTime(float Seconds);

protected:
	UPROPERTY(meta = (BindWidget)) UTextBlock* ScoreText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* TimeText;
	UPROPERTY(meta = (BindWidget)) UProgressBar* HealthBar;
	
};
