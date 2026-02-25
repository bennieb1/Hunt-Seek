// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolEnemyController.h"
#include "PatrolEnemy.h"

void APatrolEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("PatrolEnemyController possessed: %s"), *GetNameSafe(InPawn));

	const FVector Dest = InPawn->GetActorLocation() + FVector(1000.f, 0.f, 0.f);
	const auto Result = MoveToLocation(Dest, 50.f);

	UE_LOG(LogTemp, Warning, TEXT("MoveToLocation result=%d"), (int32)Result);

}

void APatrolEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{

	Super::OnMoveCompleted(RequestID, Result);

	APatrolEnemy* Enemy = Cast<APatrolEnemy>(GetPawn());
	if (!Enemy) return;

	if (Enemy->IsEngagingPlayer()) return;
	if (Enemy->Waypoints.Num() == 0) return;

	// Delay prevents immediate recursive completion loops
	const float Wait = FMath::Max(0.01f, Enemy->WaitAtWaypointSeconds);

	GetWorldTimerManager().SetTimer(
		WaitTimerHandler,
		this,
		&APatrolEnemyController::MoveToNextWaypoint,
		Wait,
		false
	);


}

void APatrolEnemyController::MoveToNextWaypoint()
{

	APatrolEnemy* Enemy = Cast<APatrolEnemy>(GetPawn());
	if (!Enemy) return;

	if (Enemy->Waypoints.Num() == 0) return;

	Enemy->CurrentWaypointIndex =
		(Enemy->CurrentWaypointIndex + 1) % Enemy->Waypoints.Num();

	AActor* Target = Enemy->Waypoints[Enemy->CurrentWaypointIndex];
	if (!Target) return;

	MoveToActor(Target, 60.f);

}

void APatrolEnemyController::WaitThenMove()
{

	MoveToNextWaypoint();

}
