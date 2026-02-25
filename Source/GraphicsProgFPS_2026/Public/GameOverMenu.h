// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverMenu.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class GRAPHICSPROGFPS_2026_API UGameOverMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetFinalTimeSeconds(float Seconds);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FinalTimeText;
	
};
