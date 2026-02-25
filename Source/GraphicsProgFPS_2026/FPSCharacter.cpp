// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"

#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EquippableToolDefinition.h"
#include "EquippableToolBase.h"
#include "FPSPlayerController.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));

	check(FirstPersonCameraComponent != nullptr);

	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FirstPersonMeshComponent != nullptr);

	FirstPersonMeshComponent->SetupAttachment(GetMesh());

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	FirstPersonMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	FirstPersonMeshComponent->SetCollisionProfileName(FName("NoCollision"));

	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, FName("head"));

	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FirstPersonCameraOffset, FRotator(0.0f, 90.0f, -90.0f));

	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = FirstPersonFieldOfView;
	FirstPersonCameraComponent->FirstPersonScale = FirstPersonScale;


}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	if (HudClass) {

		ShowHud();

	}

	FirstPersonMeshComponent->SetAnimInstanceClass(FirstPersonDefaultAnim->GeneratedClass);

	GetMesh()->SetAnimInstanceClass(FirstPersonDefaultAnim->GeneratedClass);

	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	GetMesh()->SetOwnerNoSee(true);

	GetMesh()->CastShadow = true;

	GetMesh()->bCastHiddenShadow = true;

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FirstPersonContext, 0);
		}

	}

	CurrentHealth = MaxHealth;

	if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetController()))
	{
		PC->SetHUDHealth(CurrentHealth, MaxHealth);
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are loading FPSCharacter"));
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);


		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);

		EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Use);

		if(PauseAction)
		{
			EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AFPSCharacter::TogglePauseMenu);
		}

	}

}

void AFPSCharacter::Move(const FInputActionValue& Value) 
{

	// 2D Vector of movement values returned from the input action
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Check if the controller possessing this Actor is valid
	if (Controller)
	{

		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementVector.X);

		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementVector.Y);

	}

}

void AFPSCharacter::Look(const FInputActionValue& Value)
{ 

	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}

}

void AFPSCharacter::Use(const FInputActionValue& Value)
{


	if (!EquippedTool)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No tool equipped"));
		return;
	}

	EquippedTool->Use();


}



bool AFPSCharacter::IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition)
{

	for (UEquippableToolDefinition* InventoryItem : InventoryComponent->ToolInventory)
	{

		if (ToolDefinition->ID == InventoryItem->ID)
		{
			return true;
		}

	}

	return false;
}

void AFPSCharacter::AttachTool(UEquippableToolDefinition* ToolDefinition)
{

	if (!IsToolAlreadyOwned(ToolDefinition))
	{
		// Spawn a new instance of the tool to equip
		AEquippableToolBase* ToolToEquip = GetWorld()->SpawnActor<AEquippableToolBase>(ToolDefinition->ToolAsset, this->GetActorTransform());

		// Attach the tool to the First Person Character
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		// Attach the tool to this character, and then the right hand of their first-person mesh
		ToolToEquip->AttachToActor(this, AttachmentRules);
		ToolToEquip->AttachToComponent(FirstPersonMeshComponent, AttachmentRules, FName(TEXT("HandGrip_R")));

		// Set the animations on the character's meshes.
		FirstPersonMeshComponent->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnim->GeneratedClass);
		GetMesh()->SetAnimInstanceClass(ToolToEquip->ThirdPersonToolAnim->GeneratedClass);

		// Add the tool to this character's inventory
		InventoryComponent->ToolInventory.Add(ToolDefinition);
		ToolToEquip->OwningCharacter = this;
		EquippedTool = ToolToEquip;

		// Get the player controller for this character
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(ToolToEquip->ToolMappingContext, 1);
			}
			// Bind the input action to the function
			
		}

	}

	

}


void AFPSCharacter::GiveItem(UItemDefinition* ItemDefinition)
{


	switch (ItemDefinition->ItemType)
	{

	case EItemType::Tool:
	{
		// If the item is a tool, attempt to cast and attach it to the character

		UEquippableToolDefinition* ToolDefinition = Cast<UEquippableToolDefinition>(ItemDefinition);

		if (ToolDefinition != nullptr)
		{
			AttachTool(ToolDefinition);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Cast to tool failed!"));
		}
		break;
	}

	case EItemType::Consumable:
	{
		// Not implemented
		break;
	}
	default:
		break;

	}

}

FVector AFPSCharacter::GetCameraTargetLocation()
{
	FVector TargetPosition;

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		// The result of the line trace
		FHitResult Hit;

		// Simulate a line trace from the character along the vector they're looking down
		const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
		const FVector TraceEnd = TraceStart + FirstPersonCameraComponent->GetForwardVector() * 10000.0;
		World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

		// Set the target position to the impact point of the hit or the end of the trace depending on whether it hit an object
		TargetPosition = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;

	}
	return TargetPosition;
}



void AFPSCharacter::ShowHud()
{

	if (!HudClass) return;

	if (!HudWidget)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			HudWidget = CreateWidget<UUserWidget>(PC, HudClass);
		}
	}

	if (HudWidget && !HudWidget->IsInViewport())
	{
		HudWidget->AddToViewport(0);
	}

}


void AFPSCharacter::ShowMainMenu()
{
	if (!MainMenuClass) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (!MainMenuWidget)
	{
		MainMenuWidget = CreateWidget<UUserWidget>(PC, MainMenuClass);
	}

	if (MainMenuWidget && !MainMenuWidget->IsInViewport())
	{
		MainMenuWidget->AddToViewport(10);
	}

	// input mode belongs to PC too
	PC->bShowMouseCursor = true;

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(Mode);

}

void AFPSCharacter::TogglePauseMenu()
{

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	const bool bPaused = UGameplayStatics::IsGamePaused(this);

	if (!bPaused)
	{
		UGameplayStatics::SetGamePaused(this, true);

		if (!PauseMenuClass) return;

		if (!PauseWidget)
		{
			// ✅ Owner MUST be PlayerController (or World/GameInstance), not Character
			PauseWidget = CreateWidget<UUserWidget>(PC, PauseMenuClass);
		}

		if (PauseWidget && !PauseWidget->IsInViewport())
		{
			PauseWidget->AddToViewport(20);
		}

		SetUIOnly(PauseWidget);   // make sure SetUIOnly uses PC->SetInputMode(...)
	}
	else
	{
		UGameplayStatics::SetGamePaused(this, false);

		if (PauseWidget && PauseWidget->IsInViewport())
		{
			PauseWidget->RemoveFromParent();
		}

		SetGameOnly();            // make sure SetGameOnly uses PC->SetInputMode(...)
	}
}

void AFPSCharacter::ApplyDamage(float Damage)
{

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetController()))
	{
		PC->SetHUDHealth(CurrentHealth, MaxHealth);
	}

}

float AFPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	
	ApplyDamage(DamageAmount);
	return DamageAmount;

}

void AFPSCharacter::SetUIOnly(UUserWidget* Widget)
{

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = true;

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(Widget->TakeWidget());

		PC->SetInputMode(Mode);
	}

}

void AFPSCharacter::SetGameOnly()
{

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = false;

		FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);
	}

}
