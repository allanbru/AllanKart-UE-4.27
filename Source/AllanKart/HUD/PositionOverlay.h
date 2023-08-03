// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/Public/Blueprint/IUserObjectListEntry.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "UMG/Public/Components/TextBlock.h"
#include "PositionOverlay.generated.h"

/**
 *
 */
UCLASS()
class ALLANKART_API UPositionOverlay : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* DriverPosition;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* DriverName;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* DriverDifference;

};
