// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryStructs.h"
#include "InventoryList.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ALLANKART_API UInventoryList : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryItem> ItemsList;
	
};
