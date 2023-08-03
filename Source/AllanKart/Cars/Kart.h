// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "AllanKart/LevelEditor/CheckpointActor.h"
#include "AllanKart/Types/AllanKartTypes.h"
#include "Kart.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaceFinishedSignature, AKartPlayerState*, FinishingPlayer, float, FinishingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCheckpointPassedSignature, AKartPlayerState*, Player, int32, Lap, int32, CheckpointNumber, float, RaceTime);

UCLASS()
class ALLANKART_API AKart : public AWheeledVehicle
{
	GENERATED_BODY()

public:

	AKart();

	UPROPERTY(BlueprintAssignable)
	FOnRaceFinishedSignature OnRaceFinished;

	UPROPERTY(BlueprintAssignable)
	FOnCheckpointPassedSignature OnCheckpointPassed;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void SetDriver(const FKartDriver& NewDriver);

protected:

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

private:

	void PollInit();

	UPROPERTY()
	class AKartPlayerState* KartPlayerState;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	class UAudioComponent* EngineSound;

	UPROPERTY(VisibleAnywhere, Category = "Driver")
	class USkeletalMeshComponent* DriverMesh;

	UPROPERTY(EditAnywhere, Category = "Driver")
	class UAnimMontage* CelebrationMontage;

	//Controller

	UPROPERTY()
	class AKartPlayerController* KartController;
	UPROPERTY()
	class AAIKartController* AIKartController;

	UFUNCTION(Client, Unreliable)
	void UpdateHUD();

	void ThrottleInput(float Throttle);

	void BrakeInput(float Brake);

	void SteeringInput(float Steering);

	bool bPlayingAnimation{ false };

	UFUNCTION(Server, Unreliable)
	void PlayCelebrationMontage();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayCelebrationMontage();

	// Checkpoints
	UPROPERTY(VisibleAnywhere)
	int32 CurrentCheckpoint{ 0 };

	// Lap and timing

	int32 CurrentLap{ 0 };
	float FastestLap{ 0.f };
	float TimeOffset{ 0.f };

	int32 NumberOfCheckpoints{ -1 };
	int32 NumberOfLaps{ -1 };

	// Sound
	void UpdateEngineSound();

	// Start
	void SetSpawnPoint();

	// Driver
	UFUNCTION(Server, Reliable)
	void ServerSetDriver(const FKartDriver& NewDriver);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetDriver(const FKartDriver& NewDriver);

public:

	void SetCurrentCheckpoint(int32 Checkpoint, ECheckpointType CheckpointType);
	FORCEINLINE int32 GetCheckpointNumber() const { return CurrentCheckpoint; }
	FORCEINLINE int32 GetNumberOfCheckpoints() const { return NumberOfCheckpoints;  }
	FORCEINLINE void SetNumberOfCheckpoints(int32 NewNumberOfCheckpoints) { NumberOfCheckpoints = NewNumberOfCheckpoints;  }
	FORCEINLINE void SetNumberOfLaps(int32 NewNumberOfLaps) { NumberOfLaps = NewNumberOfLaps; }

};
