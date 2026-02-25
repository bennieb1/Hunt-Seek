// Fill out your copyright notice in the Description page of Project Settings.


#include "DartLaunchers.h"// Changed from FPSCharacter.generated.h
#include  "GameFramework/Character.h"
#include "GraphicsProgFPS_2026/FPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "FirstPersonProjectile.h"
#include "InputAction.h"

void ADartLaunchers::Use()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Using the dart launcher!"));


	UWorld* const World = GetWorld();
	if (World != nullptr && ProjectileClass != nullptr)
	{
		FVector TargetPosition = OwningCharacter->GetCameraTargetLocation();

		// Get the correct socket to spawn the projectile from
		FVector SocketLocation = ToolMeshComponent->GetSocketLocation("Muzzle");
		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, TargetPosition);
		FVector SpawnLocation = SocketLocation + UKismetMathLibrary::GetForwardVector(SpawnRotation) * 10.0;

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// Spawn the projectile at the muzzle
		World->SpawnActor<AFirstPersonProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}

}

// Called every frame
void ADartLaunchers::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

