// Fill out your copyright notice in the Description page of Project Settings.


#include "KartOverlay.h"

void UKartOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	if(PositionWidgetClass)
	{
		PositionOverlay.Empty();
		for(int32 i=0; i<PositionsToShow; i++)
		{
			UPositionOverlay* PositionWidget = CreateWidget<UPositionOverlay>(this, PositionWidgetClass, FName("PositionWidget"));
			if (PositionWidget)
			{
				PositionWidget->SetVisibility(ESlateVisibility::Visible);
				NearestPositions->AddChildToVerticalBox(PositionWidget);
				PositionOverlay.Add(PositionWidget);
			}
		}
	}
}
