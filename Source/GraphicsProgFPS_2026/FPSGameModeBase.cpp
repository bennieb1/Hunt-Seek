// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameModeBase.h"

void AFPSGameModeBase::StartPlay()
{

	Super::StartPlay();

	check(GEngine != nullptr);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Welcome to FPS Game!"));
}
