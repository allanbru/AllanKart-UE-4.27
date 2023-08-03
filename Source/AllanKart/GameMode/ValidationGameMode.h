// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RaceModesInterface.h"
#include "GameFramework/GameMode.h"
#include "ValidationGameMode.generated.h"

class AKartPlayerState;
/**
 *
 */
UCLASS()
class ALLANKART_API AValidationGameMode : public AGameMode, public IRaceModesInterface
{
	GENERATED_BODY()

public:

	AValidationGameMode();

	UFUNCTION()
	void StartRace();

	UFUNCTION()
	void PassedCheckpoint_Implementation(AKartPlayerState* PlayerState) override;

	UFUNCTION()
	void FinishedRace_Implementation(AKartPlayerState* Player, float TotalTime) override;

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
    class AGround* Ground;

	UPROPERTY(EditAnywhere, Category="Defaults")
	float StartDelay{5.f};

	FTimerHandle StartTimerHandle;

};
