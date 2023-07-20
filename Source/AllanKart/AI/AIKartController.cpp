// Fill out your copyright notice in the Description page of Project Settings.


#include "AIKartController.h"

#include "WheeledVehicleMovementComponent.h"
#include "AllanKart/Cars/Kart.h"
#include "AllanKart/GameMode/RaceGameMode.h"
#include "AllanKart/LevelEditor/Ground.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

AAIKartController::AAIKartController()
{
	PrimaryActorTick.bCanEverTick = true;
	bWantsPlayerState = true;
}

void AAIKartController::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(AllowRewardTimerHandle, this, &AAIKartController::AllowReward, 1.f);
}

void AAIKartController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AccumulatedTime += DeltaTime;
	if(bShouldRestart && AccumulatedTime > ResetInterval)
	{
		AccumulatedTime = 0.f;
		ResetToLastCheckpoint();
		StoredReward = 0.f;
	}
}

bool AAIKartController::FindGround()
{
	if(Ground == nullptr)
	{
		AActor* Candidate = UGameplayStatics::GetActorOfClass(this, AGround::StaticClass());
		if(Candidate)
		{
			AGround* PotentialGround = Cast<AGround>(Candidate);
			if(PotentialGround)
			{
				Ground = PotentialGround;
			}
		}
	}
	return Ground != nullptr;
}

void AAIKartController::RespawnPlayerAtTransformAtEndOfTimer()
{
	GameMode->RestartPlayerAtTransform(this, CheckpointTransform);
	ControlledKart->SetActorTransform(CheckpointTransform, false, nullptr, ETeleportType::TeleportPhysics);
	bAwaitingRespawn = false;
	bCanReward = false;
	GetWorldTimerManager().SetTimer(AllowRewardTimerHandle, this, &AAIKartController::AllowReward, 1.f);
}

void AAIKartController::AllowReward()
{
	bAwaitingRespawn = false;
	bCanReward = true;
}

void AAIKartController::NotifyWaypointIntersection(const int32 WaypointNumber)
{
	if(WaypointNumber > CurrentWaypoint || (WaypointNumber == 0 && CurrentWaypoint == NumberOfWaypoints - 1))
	{
		CurrentWaypoint = WaypointNumber;
		StoredReward += WaypointsReward;
	}
	else if(WaypointNumber < CurrentWaypoint)
	{
		StoredReward -= WaypointsPenalty;
	}
}

void AAIKartController::ResetToLastCheckpoint()
{
	if(bAwaitingRespawn) return;
	GameMode = (GameMode == nullptr) ? GetWorld()->GetAuthGameMode() : GameMode;
	if(GetPawn() == nullptr || GameMode == nullptr) return;
	ControlledKart = (ControlledKart == nullptr) ? Cast<AKart>(GetPawn()) : ControlledKart;
	if(ControlledKart && FindGround())
	{
		const int32 CurrentCheckpoint = ControlledKart->GetCheckpointNumber();
		CheckpointTransform = Ground->GetCheckpointTransform(CurrentCheckpoint);
		const FVector Offset = FVector::ZAxisVector * 50.f;
		CheckpointTransform.SetLocation(CheckpointTransform.GetLocation() + Offset);
		bAwaitingRespawn = true;
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AAIKartController::RespawnPlayerAtTransformAtEndOfTimer, FMath::FRandRange(0.f, 2.f));
	}
}

float AAIKartController::CalculateReward()
{
	if(bAwaitingRespawn || !bCanReward) return 0.f;
	ControlledKart = (ControlledKart == nullptr) ? Cast<AKart>(GetPawn()) : ControlledKart;

	if(ControlledKart && FindGround())
	{
		const float Speed = ControlledKart->GetVehicleMovement()->GetForwardSpeed();
		const float OffCenter = -AllowOffCenter + Ground->GetDistanceToSpline(ControlledKart->GetActorLocation());
		const float NewReward = StoredReward + SpeedWeight * Speed - OffCenterWeight * OffCenter;
		StoredReward = 0.f;
		return NewReward;
	}
	
	StoredReward = 0.f;
	return 0.f;
}

void AAIKartController::UpdateLeaderboard()
{
	if(FindGameMode() && FindPlayerState())
	{
		RaceGameMode->PassedCheckpoint(KartPlayerState);
		StoredReward += 100.f;
	}
}

bool AAIKartController::FindGameMode()
{
	GameMode = (GameMode == nullptr) ? GetWorld()->GetAuthGameMode() : GameMode;
	if(GameMode)
	{
		RaceGameMode = (RaceGameMode == nullptr) ? Cast<ARaceGameMode>(GameMode) : RaceGameMode;
		return RaceGameMode != nullptr;
	}
	return false;
}

bool AAIKartController::FindPlayerState()
{
	if(KartPlayerState) return true;
	KartPlayerState = GetPlayerState<AKartPlayerState>();
	return KartPlayerState != nullptr;
}
