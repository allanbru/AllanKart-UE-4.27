// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RaceModesInterface.h"
#include "AllanKart/Types/LeaderboardPosition.h"
#include "GameFramework/GameMode.h"
#include "RaceGameMode.generated.h"

class AKartPlayerState;
class ARaceGameState;
/**
 *
 */
UCLASS()
class ALLANKART_API ARaceGameMode : public AGameMode, public IRaceModesInterface
{
	GENERATED_BODY()

public:
	ARaceGameMode();

	UPROPERTY(EditDefaultsOnly)
	FString TrackTempFileName{"__CurrentPlayingMap__"};

	UFUNCTION()
	void PassedCheckpoint_Implementation(AKartPlayerState* PlayerState);

	UFUNCTION()
	void FinishedRace_Implementation(AKartPlayerState* Player, float TotalTime);

protected:
	virtual void BeginPlay() override;
	virtual void HandleMatchHasStarted() override;

private:

	UPROPERTY()
	ARaceGameState* RaceGameState;

	UPROPERTY()
	class AGround* Ground;

	UPROPERTY()
	int32 NumberOfCheckpoints{0};

	UPROPERTY(EditAnywhere, Category="Defaults")
	TSubclassOf<APawn> AIPawnClass;

	UPROPERTY(EditAnywhere, Category="Defaults")
	int32 DesiredNumberOfPlayers{3};

	UPROPERTY(EditAnywhere, Category="Defaults")
	float StartDelay{5.f};

	FTimerHandle StartTimerHandle;

	UPROPERTY(EditAnywhere)
	float GridDistance{300.f};

	UFUNCTION()
	void StartRace();

	float ServerStartTime{0.f};

public:

	FORCEINLINE float GetServerTime() const { return GetWorld()->GetTimeSeconds() - ServerStartTime; }

};
