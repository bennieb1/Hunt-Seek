// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverMenu.h"
#include "Components/TextBlock.h"

void UGameOverMenu::SetFinalTimeSeconds(float Seconds)
{

	Seconds = FMath::Max(0.f, Seconds);

	const int32 Total = FMath::FloorToInt(Seconds);
	const int32 Minutes = Total / 60;
	const int32 Remainder = Total % 60;

	if (FinalTimeText)
	{
		FinalTimeText->SetText(FText::FromString(
			FString::Printf(TEXT("Total Time: %02d:%02d"), Minutes, Remainder)
		));
	}

}

void UGameOverMenu::SetResultMessage(const FString& Message)
{

	if (ResultText)
	{
		ResultText->SetText(FText::FromString(Message));
	}

}
