// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "KartOverlay.h"
#include "KartHUD.generated.h"

/**
 * 
 */
UCLASS()
class ALLANKART_API AKartHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UKartOverlay> OverlayWidgetClass;
	
	UPROPERTY()
	UKartOverlay* KartOverlay;
	

};
