// Fill out your copyright notice in the Description page of Project Settings.


#include "KartPlayerState.h"

#include "AllanKart/AI/AIKartController.h"
#include "AllanKart/PlayerController/KartPlayerController.h"
#include "Net/UnrealNetwork.h"


AKartPlayerState::AKartPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AKartPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

bool AKartPlayerState::FindController()
{
	if(IsABot())
	{
		if(AIKartController) return true;
		if(GetPawn() && GetPawn()->GetController())
		{
			AIKartController = Cast<AAIKartController>(GetPawn()->GetController());
			return AIKartController != nullptr;
		}
		return false;
	}

	if(KartPlayerController) return true;
	if(GetPawn() && GetPawn()->GetController())
	{
		KartPlayerController = Cast<AKartPlayerController>(GetPawn()->GetController());
		return KartPlayerController != nullptr;
	}
	return false;
}

void AKartPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKartPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKartPlayerState, CurrentCheckpoint);
	DOREPLIFETIME(AKartPlayerState, CurrentLap);
}

void AKartPlayerState::ServerSetCurrentCheckpoint_Implementation(const int32 CheckpointNumber, const float RaceTime)
{
	CurrentCheckpoint = CheckpointNumber;
	SetScore(CurrentCheckpoint + CheckpointsInLap * CurrentLap);
	RaceTimeAtScore.Emplace(GetScore(), RaceTime);
	if(FindController())
	{
		if(IsABot())
		{
			AIKartController->UpdateLeaderboard();
		}
		else
		{
			KartPlayerController->UpdateLeaderboard();
		}
	}
}

void AKartPlayerState::SetCurrentLap(const int32 LapNumber)
{
	CurrentLap = LapNumber;
}

float AKartPlayerState::GetRaceTimeInScore(const int32 DesiredScore) const
{
	if(RaceTimeAtScore.Contains(DesiredScore))
	{
		return RaceTimeAtScore[DesiredScore];
	}
	return -INFINITY;
}

void AKartPlayerState::MulticastSetCheckpointsInLap_Implementation(const int32 NumCheckpoints)
{
	CheckpointsInLap = NumCheckpoints;
}
