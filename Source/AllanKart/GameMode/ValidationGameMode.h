// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ValidationGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ALLANKART_API AValidationGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	AValidationGameMode();

	UFUNCTION()
	void StartRace();

	UFUNCTION()
	void PassedCheckpoint(AKart* Player, int32 Lap, int32 Checkpoint, float TotalTime);

	UFUNCTION()
	void FinishedRace(AKart* Player, float TotalTime);

protected:

	virtual void BeginPlay() override;
	
private:

	UPROPERTY()
    class AGround* Ground;
	
	UPROPERTY(EditAnywhere, Category="Defaults")
	float StartDelay{5.f};

	FTimerHandle StartTimerHandle;
		
};
