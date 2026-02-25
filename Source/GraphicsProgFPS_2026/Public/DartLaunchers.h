// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquippableToolBase.h"
#include "DartLaunchers.generated.h"

class UInputAction;
class AFirstPersonProjectile;
class AFPSCharacter;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GRAPHICSPROGFPS_2026_API ADartLaunchers : public AEquippableToolBase
{
	GENERATED_BODY()


public:

	virtual void Use() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<AFirstPersonProjectile> ProjectileClass;

};

	
