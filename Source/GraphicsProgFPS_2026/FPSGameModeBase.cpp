// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameModeBase.h"
#include "EngineUtils.h"
#include "PatrolEnemy.h"
#include "FPSPlayerController.h"

void AFPSGameModeBase::StartPlay()
{

	Super::StartPlay();

	int32 Count = 0;
	for (TActorIterator<APatrolEnemy> It(GetWorld()); It; ++It)
	{
		if (IsValid(*It))
		{
			Count++;
		}
	}

	AliveEnemies = Count;
	bGameOverTriggered = false;

	UE_LOG(LogTemp, Warning, TEXT("FPSGameModeBase StartPlay: AliveEnemies=%d"), AliveEnemies);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Yellow,
			FString::Printf(TEXT("Welcome to FPS Game! Enemies: %d"), AliveEnemies)
		);
	}

}

void AFPSGameModeBase::NotifyEnemyKilled(AController* Killer)
{

	if (bGameOverTriggered) return;

	AliveEnemies = FMath::Max(AliveEnemies - 1, 0);

	UE_LOG(LogTemp, Warning, TEXT("Enemy killed. AliveEnemies now: %d"), AliveEnemies);

	if (AliveEnemies == 0)
	{
		bGameOverTriggered = true;

		AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetWorld()->GetFirstPlayerController());
		if (!PC)
		{
			UE_LOG(LogTemp, Warning, TEXT("NotifyEnemyKilled: No FPSPlayerController found"));
			return;
		}

		const float FinalTime = PC->GetElapsedHudTimeSeconds();
		PC->ShowGameOver(FinalTime, true);
	}

}
