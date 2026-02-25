// Fill out your copyright notice in the Description page of Project Settings.


#include "HudMenu.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UHudMenu::SetScore(int32 NewScore)
{

	if (ScoreText)
		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), NewScore)));

}

void UHudMenu::SetHealth(float Current, float Max)
{

	const float ClampedMax = FMath::Max(Max, 1.0f);
	const float Percent = FMath::Clamp(Current / ClampedMax, 0.0f, 1.0f);

	if (HealthBar) HealthBar->SetPercent(Percent);

	if (HealthText)
	{
		HealthText->SetText(FText::FromString(FString::Printf(
			TEXT("Health: %d / %d"),
			FMath::RoundToInt(Current),
			FMath::RoundToInt(ClampedMax)
		)));
	}

}

void UHudMenu::SetElapsedTime(float Seconds)
{

	Seconds = FMath::Max(0.0f, Seconds);

	const int32 Total = FMath::FloorToInt(Seconds);
	const int32 Minutes = Total / 60;
	const int32 Remainder = Total % 60;

	if (TimeText)
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %02d:%02d"), Minutes, Remainder)));

}
