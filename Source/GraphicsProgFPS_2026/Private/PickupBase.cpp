// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupBase.h"
#include "ItemDefinition.h"

// Sets default values
APickupBase::APickupBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create this pickup's mesh component
	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	check(PickupMeshComponent != nullptr);

	// Create this pickup's sphere component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	check(SphereComponent != nullptr);

	// Attach the sphere component to the mesh component
	SphereComponent->SetupAttachment(PickupMeshComponent);

	// Set the sphere's collision radius
	SphereComponent->SetSphereRadius(69.0f);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	// Initialize this pickup with default values
	InitializePickup();
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/**
*	Initializes the pickup with default values by retrieving them from the associated data table.
*/
void APickupBase::InitializePickup()
{
	if (PickupDataTable && !PickupItemID.IsNone())
	{
		// Retrieve the item data associated with this pickup from the Data Table
		const FItemData* ItemDataRow = PickupDataTable->FindRow<FItemData>(PickupItemID, PickupItemID.ToString());

		ReferenceItem = NewObject<UItemDefinition>(this, UItemDefinition::StaticClass());

		ReferenceItem->ID = ItemDataRow->ID;
		ReferenceItem->ItemType = ItemDataRow->ItemType;
		ReferenceItem->ItemText = ItemDataRow->ItemText;
		ReferenceItem->WorldMesh = ItemDataRow->ItemBase->WorldMesh;

		UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase.Get();

		

		// Check if the mesh is currently loaded by calling IsValid().
		if (TempItemDefinition->WorldMesh.IsValid()) {
			// Set the pickup's mesh to the associated item's mesh
			PickupMeshComponent->SetStaticMesh(TempItemDefinition->WorldMesh.Get());
		}
		else {
			// If the mesh isn't loaded, load it by calling LoadSynchronous().
			UStaticMesh* WorldMesh = TempItemDefinition->WorldMesh.LoadSynchronous();
			PickupMeshComponent->SetStaticMesh(WorldMesh);
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Picked up Object"));
		// Set the mesh to visible and collidable.
		PickupMeshComponent->SetVisibility(true);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// Register the Overlap Event
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnSphereBeginOverlap);
	}
}

/**
*	Broadcasts an event when a character overlaps this pickup's SphereComponent. Sets the pickup to invisible and uninteractable, and respawns it after a set time.
*	@param OverlappedComponent - the component that was overlapped.
*	@param OtherActor - the Actor overlapping this component.
*	@param OtherComp - the Actor's component that overlapped this component.
*	@param OtherBodyIndex - the index of the overlapped component.
*	@param bFromSweep - whether the overlap was generated from a sweep.
*	@param SweepResult - contains info about the overlap such as surface normals and faces.
*/
void APickupBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Attempting a pickup collision"));

	if (!SphereComponent || !PickupMeshComponent)
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Pickup Component"));
		return;

	}

	if (OtherActor)
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
			FString::Printf(TEXT("Actor: %s"), *OtherActor->GetName())
		);

	}

	if (OtherComp)
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
			FString::Printf(TEXT("Component: %s"), *OtherComp->GetName())
		);

	}

	TArray<AActor*> OverlappingActors;
	SphereComponent->GetOverlappingActors(OverlappingActors);

	GEngine->AddOnScreenDebugMessage(
		-1, 5.0f, FColor::Purple,
		FString::Printf(TEXT("Overlapping count: %d"), OverlappingActors.Num())
	);

	AFPSCharacter* Character = Cast<AFPSCharacter>(OtherActor);
	if (Character)
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Picked up Object"));
		SphereComponent->OnComponentBeginOverlap.RemoveAll(this);
		PickupMeshComponent->SetVisibility(false);
		PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (bShouldRespawn)
	{

		GetWorldTimerManager().SetTimer(
			RespawnTimerHandle,
			this,
			&APickupBase::InitializePickup,
			RespawnTime,
			false
		);

	}
}

/**
*	Updates this pickup whenever a property is changed.
*	@param PropertyChangedEvent - contains info about the property that was changed.
*/
void APickupBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Handle parent class property changes
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// If a property was changed, get the name of the changed property. Otherwise use none.
	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Verify that the changed property exists in this class and that the PickupDataTable is valid.
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickupBase, PickupItemID) && PickupDataTable)
	{
		// Retrieve the associated ItemData for this pickup.
		if (const FItemData* ItemDataRow = PickupDataTable->FindRow<FItemData>(PickupItemID, PickupItemID.ToString()))
		{
			UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase.Get();

			// Set the pickup's mesh to the associated item's mesh
			PickupMeshComponent->SetStaticMesh(TempItemDefinition->WorldMesh.Get());

			// Set the sphere's collision radius
			SphereComponent->SetSphereRadius(32.f);
		}
	}
}


