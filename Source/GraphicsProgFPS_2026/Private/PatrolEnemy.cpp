// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolEnemy.h"
#include "Perception/PawnSensingComponent.h"
#include "FirstPersonProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "FPSPlayerController.h"
#include "../FPSGameModeBase.h"

// Sets default values
APatrolEnemy::APatrolEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 2000.f;
	PawnSensing->SetPeripheralVisionAngle(60.f);

	// Weapon mesh (visual)
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GetMesh()); // we’ll attach to socket at runtime
	GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Muzzle point (reliable projectile spawn)
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(GunMesh);

}

// Called when the game starts or when spawned
void APatrolEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	// Attach gun to hand socket
	if (GetMesh() && GunMesh)
	{
		GunMesh->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			GunAttachSocketName
		);
	}

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &APatrolEnemy::OnSeePawn);
	}

	// For headshot detection (bone name)
	OnTakePointDamage.AddDynamic(this, &APatrolEnemy::OnPointDamageTaken);
	
}

void APatrolEnemy::OnSeePawn(APawn* Pawn)
{

	if (!Pawn) return;

	SeenPlayer = Pawn;
	bEngagingPlayer = true;

	// If we keep seeing player, don't drop engagement
	GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);

	StartFiring();

}

void APatrolEnemy::LosePlayer()
{
	bEngagingPlayer = false;
	SeenPlayer.Reset();
	StopFiring();

}

void APatrolEnemy::StartFiring()
{

	if (!ProjectileClass) return;

	// Fire instantly and then on timer
	FireOnce();

	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&APatrolEnemy::FireOnce,
		FireRateSeconds,
		true
	);

	// If we don’t see player again for a bit, go back to patrol
	GetWorldTimerManager().SetTimer(
		LoseSightTimerHandle,
		this,
		&APatrolEnemy::LosePlayer,
		LoseSightDelaySeconds,
		false
	);

}

void APatrolEnemy::StopFiring()
{

	GetWorldTimerManager().ClearTimer(FireTimerHandle);

}

void APatrolEnemy::FireOnce()
{

	if (!ProjectileClass) return;
	if (!SeenPlayer.IsValid()) return;

	const FVector SpawnLoc = (MuzzlePoint ? MuzzlePoint->GetComponentLocation()
		: GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0, 0, 50.f));

	const FVector TargetLoc = SeenPlayer->GetActorLocation() + FVector(0, 0, 50.f);
	const FRotator SpawnRot = (TargetLoc - SpawnLoc).Rotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	AFirstPersonProjectile* Proj = GetWorld()->SpawnActor<AFirstPersonProjectile>(
		ProjectileClass, SpawnLoc, SpawnRot, Params
	);

	if (Proj)
	{
		Proj->Damage = ProjectileDamage;
	}

}

float APatrolEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		Die(EventInstigator);
	}

	return DamageAmount;


}

void APatrolEnemy::OnPointDamageTaken(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{

	// Treat the LAST hit as a headshot if the bone matches.
	// IMPORTANT: set HeadBoneName to match your skeleton (e.g. "Head", "b_Head", etc.)
	bLastHitWasHeadshot = (BoneName == HeadBoneName);

}

void APatrolEnemy::Die(AController* Killer)
{

	// Award score based on whether the killing shot was a headshot
	const int32 Award = bLastHitWasHeadshot ? HeadshotKillScore : BodyKillScore;

	if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->AddScore(Award);
	}

	// Notify GameMode (for “all enemies dead” -> game over)
	if (AFPSGameModeBase* GM = Cast<AFPSGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GM->NotifyEnemyKilled(Killer);
	}

	Destroy();

}

// Called every frame
void APatrolEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APatrolEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

