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
	
};
