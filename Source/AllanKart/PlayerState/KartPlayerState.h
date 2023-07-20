// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "KartPlayerState.generated.h"

class AAIKartController;
class AKartPlayerController;

UCLASS()
class ALLANKART_API AKartPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	AKartPlayerState();
	
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCheckpointsInLap(const int32 NumCheckpoints);

	UFUNCTION(Server, Reliable)
	void ServerSetCurrentCheckpoint(const int32 CheckpointNumber, const float RaceTime);
	
protected:
	virtual void BeginPlay() override;

private:

	TMap<int32, float> RaceTimeAtScore;

	UPROPERTY()
	AKartPlayerController* KartPlayerController;

	UPROPERTY()
	AAIKartController* AIKartController;
	bool FindController();
	
	int32 CheckpointsInLap{0};

	UPROPERTY(Replicated)
	int32 CurrentCheckpoint{0};

	UPROPERTY(Replicated)
	int32 CurrentLap{0};

public:

	void SetCurrentLap(const int32 LapNumber);
	FORCEINLINE int32 GetCurrentCheckpoint() const { return CurrentCheckpoint; }
	FORCEINLINE int32 GetCurrentLap() const { return CurrentLap; }

	float GetRaceTimeInScore(const int32 DesiredScore) const;
};
