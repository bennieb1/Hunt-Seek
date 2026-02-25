#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GRAPHICSPROGFPS_2026_API AFPSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void StartPlay() override;
	
public:

	void NotifyEnemyKilled(AController* Killer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WinCondition")
	int32 AliveEnemies = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WinCondition")
	bool bGameOverTriggered = false;



};
