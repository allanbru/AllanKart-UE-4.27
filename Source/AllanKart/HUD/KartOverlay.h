// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "PositionOverlay.h"
#include "Components/ListView.h"
#include "KartOverlay.generated.h"

/**
 * 
 */
UCLASS()
class ALLANKART_API UKartOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Timer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Speed;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Gear;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EngineRev;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Laps;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FastestLap;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPositionOverlay> PositionWidgetClass;

	UPROPERTY(EditAnywhere)
	int32 PositionsToShow{5};
	
	UPROPERTY(VisibleAnywhere)
	TArray<UPositionOverlay*> PositionOverlay;

	//UPROPERTY(meta = (BindWidget))
	UPROPERTY()
	UListView* PositionsList;
	
};
