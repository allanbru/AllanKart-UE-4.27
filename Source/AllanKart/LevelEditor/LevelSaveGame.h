// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Ground.h"
#include "LevelSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ALLANKART_API ULevelSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTrackPoints TrackPoints;
	
};
