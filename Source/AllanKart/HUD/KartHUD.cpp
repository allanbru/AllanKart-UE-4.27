// Fill out your copyright notice in the Description page of Project Settings.


#include "KartHUD.h"

void AKartHUD::BeginPlay()
{
	Super::BeginPlay();

	if(OverlayWidgetClass)
	{
		KartOverlay = CreateWidget<UKartOverlay>(GetOwningPlayerController(), OverlayWidgetClass, FName("KartOverlay"));
		if (KartOverlay)
		{
			KartOverlay->AddToViewport();
		}
	}
}
