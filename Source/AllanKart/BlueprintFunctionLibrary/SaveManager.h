// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AllanKart/Types/TrackPoints.h"

#include "SaveManager.generated.h"


class AGround;
class ULevelSaveGame;
/**
 *
 */
UCLASS()
class ALLANKART_API USaveManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

    UFUNCTION(BlueprintCallable)
    static bool SaveLevel(FTrackPoints TrackPoints, const FText& TrackName, const uint8 NumberOfLaps);

    UFUNCTION(BlueprintCallable)
    static bool LoadLevel(const FText& TrackName, FTrackPoints& TrackPoints);

    UFUNCTION(BlueprintCallable)
    static AGround* FindGround(const UObject* WorldContextObject, bool& FindResult);

};
