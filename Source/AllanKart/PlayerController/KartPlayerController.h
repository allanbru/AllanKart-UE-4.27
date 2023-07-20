// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KartPlayerController.generated.h"

class AKartPlayerState;
class ARaceGameState;
/**
 * 
 */
UCLASS()
class ALLANKART_API AKartPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Client, Reliable)
	void SetHUDTimer(float Time);
	UFUNCTION(Client, Reliable)
	void SetHUDFastestLap(float Time);
	void SetHUDSpeed(float Speed);
	void SetHUDGear(int32 Gear);
	UFUNCTION(Client, Reliable)
	void SetHUDLaps(int32 Laps);
	void SetHUDRevs(float Revs, float MaxRevs);
	/**
	 * @brief 
	 * @param PositionInScreen 0 means first, NumDisplayedPositions-1 last
	 * @param Position Real Position
	 * @param PlayerName Player Name
	 * @param TimeDiff Difference to leader
	 */
	UFUNCTION(Client, Reliable)
	void SetHUDPosition(const int32 PositionInScreen, const int32 Position, const FString& PlayerName, const float TimeDiff);

	UFUNCTION()
	void UpdateLeaderboard();

	UFUNCTION(BlueprintCallable)
	void ResetToLastCheckpoint();

	UFUNCTION(Server, Reliable)
	void ServerResetToLastCheckpoint();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastResetToLastCheckpoint();

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(const float TimeOfClientRequest, const float TimeServerReceivedClientRequest);

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(const float TimeOfClientRequest);
	
	float GetServerTime() const;

private:

	void PollInit();

	UPROPERTY()
	class AKart* PlayerKart;

	UPROPERTY()
	class AKartHUD* KartHUD;

	UPROPERTY()
	AGameModeBase* GameMode;

	UPROPERTY()
	AKartPlayerState* KartPlayerState;
	
	UPROPERTY()
	ARaceGameState* RaceGameState;

	bool FindGameState();

	UPROPERTY()
	class ARaceGameMode* RaceGameMode;
	
	bool FindGameMode();

	FTimerHandle ChronometerTimer;

	UFUNCTION()
	void ClearTimer();

	UPROPERTY()
	class AGround* Ground;

	bool FindGround();

	FTransform GridSlot;

	/**
	 * Lag and other stuff 
	 */

	float SingleTripTime{0.f};
	float ClientServerDelta{0.f};
	
	float TimeSyncRunningTime{0.f};
	float TimeSyncDeltaTime{0.f};
	float LevelStartingTime{0.f};
	void CheckTimeSync(const float DeltaSeconds);

public:

	void SetGridSlot(const FTransform& NewGridSlot);
	FORCEINLINE FTransform GetGridSlot() const { return GridSlot; }
};
