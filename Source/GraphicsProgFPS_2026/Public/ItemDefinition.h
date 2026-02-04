// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.h"
#include "ItemDefinition.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GRAPHICSPROGFPS_2026_API UItemDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;

	// The type of the item.
	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	// Text struct including the item name and description.
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemText ItemText;

	// --- New Code Start --- 

	// The Static Mesh used to display this item in the world.
	UPROPERTY(EditAnywhere, Category = "Item Data")
	TSoftObjectPtr<UStaticMesh> WorldMesh;
	

};
