#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PatrolEnemyController.generated.h"

UCLASS()
class GRAPHICSPROGFPS_2026_API APatrolEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	// Called when MoveTo finishes
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	void MoveToNextWaypoint();
	void WaitThenMove();

	FTimerHandle WaitTimerHandler;
};