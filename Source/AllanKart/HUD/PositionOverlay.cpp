// Fill out your copyright notice in the Description page of Project Settings.


#include "PositionOverlay.h"


void UPositionOverlay::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPositionOverlay::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	return;
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	const UPositionOverlay* NewObj = Cast<UPositionOverlay>(ListItemObject);
	if(NewObj)
	{
		DriverPosition->SetText(NewObj->DriverPosition->GetText());
		DriverName->SetText(NewObj->DriverName->GetText());
		DriverDifference->SetText(NewObj->DriverDifference->GetText());
	}
}
