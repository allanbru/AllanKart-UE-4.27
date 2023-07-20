// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIKartController.generated.h"

UCLASS()
class ALLANKART_API AAIKartController : public AAIController
{
	GENERATED_BODY()

public:
	AAIKartController();
	virtual void Tick(float DeltaTime) override;

	void UpdateLeaderboard();

	UFUNCTION(BlueprintCallable)
	void ResetToLastCheckpoint();

	UFUNCTION()
	void AllowReward();
	
	UFUNCTION(BlueprintCallable)
	float CalculateReward();

	UPROPERTY(EditAnywhere)
	bool BackwardWaypointsPenalize{true};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentWaypoint{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfWaypoints{0};

	UPROPERTY(EditAnywhere)
	float WaypointsReward{10.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaypointsPenalty{10.f};

	UPROPERTY(EditAnywhere)
	float SpeedWeight{.1f};

	UPROPERTY(EditAnywhere)
	float OffCenterWeight{.3f};

	UPROPERTY(EditAnywhere)
	float AllowOffCenter{100.f};
	
	// Reward to be given in the next call
    UPROPERTY(BlueprintReadWrite)
    float StoredReward{0.f};
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class AKart* ControlledKart;

	UPROPERTY()
	class AGameModeBase* GameMode;

	UPROPERTY()
	class ARaceGameMode* RaceGameMode;
	bool FindGameMode();

	UPROPERTY()
	class AKartPlayerState* KartPlayerState;
	bool FindPlayerState();
	
	UPROPERTY()
	class AGround* Ground;
	
	bool FindGround();

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	bool bAwaitingRespawn{false};
	
	FTimerHandle RespawnTimerHandle;
	FTransform CheckpointTransform;
	
	UFUNCTION()
	void RespawnPlayerAtTransformAtEndOfTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bShouldRestart{true};

	FTimerHandle AllowRewardTimerHandle;
	bool bCanReward{false};	
	
	// Reset kart every defined amount of seconds
	UPROPERTY(EditAnywhere)
	float ResetInterval{20.f};

	float AccumulatedTime{0.f};
	
public:
	void NotifyWaypointIntersection(const int32 WaypointNumber);

};
