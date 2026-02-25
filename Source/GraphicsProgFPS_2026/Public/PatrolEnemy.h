// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolEnemy.generated.h"

class UPawnSensingComponent;
class AFirstPersonProjectile;
class AFPSGameModeBase;

UCLASS()
class GRAPHICSPROGFPS_2026_API APatrolEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APatrolEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> GunMesh;

	// Muzzle transform comes from this component (attached to gun)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> MuzzlePoint;

	// Name of the socket on the character skeletal mesh to attach the gun to.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName GunAttachSocketName = TEXT("hand_rSocket");

	// Projectile class to spawn when shooting (assign in BP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AFirstPersonProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float FireRateSeconds = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float ProjectileDamage = 10.f;

	// =========================
	// AI PATROL
	// =========================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol")
	TArray<TObjectPtr<AActor>> Waypoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol")
	float WaitAtWaypointSeconds = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Patrol")
	int32 CurrentWaypointIndex = 0;

	// Helper for controller
	bool IsEngagingPlayer() const { return bEngagingPlayer; }

	// =========================
	// AI SENSING / COMBAT
	// =========================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Sensing")
	TObjectPtr<UPawnSensingComponent> PawnSensing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat")
	float LoseSightDelaySeconds = 2.0f;

	// =========================
	// HEALTH + SCORING
	// =========================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Health")
	float CurrentHealth = 100.f;

	// Headshot scoring
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Score")
	int32 BodyKillScore = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Score")
	int32 HeadshotKillScore = 200;

	// Must match your skeleton bone name (common: "head", "Head", "b_Head")
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Score")
	FName HeadBoneName = TEXT("head");

protected:
	virtual void BeginPlay() override;

	// Vision event
	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	// Bone-name aware hit (headshots)
	UFUNCTION()
	void OnPointDamageTaken(AActor* DamagedActor, float Damage, AController* InstigatedBy,
		FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
		FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	// Generic damage (for ApplyPointDamage, this still gets called)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

private:
	// Combat state
	bool bEngagingPlayer = false;
	TWeakObjectPtr<APawn> SeenPlayer;

	bool bLastHitWasHeadshot = false;

	FTimerHandle FireTimerHandle;
	FTimerHandle LoseSightTimerHandle;

	void StartFiring();
	void StopFiring();
	void FireOnce();

	void LosePlayer();

	void Die(AController* Killer);
	void Tick(float DeltaTime);
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
};
