// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceGameState.h"

#include "AllanKart/LevelEditor/Ground.h"
#include "AllanKart/PlayerState/KartPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ARaceGameState::ARaceGameState()
{
	bReplicates = true;
}

void ARaceGameState::BuildTrack(const FTrackPoints& NewTrackPoints)
{
	TrackPoints = NewTrackPoints;

	if(HasAuthority())
	{
		UE_LOG(LogTemp,Warning,TEXT("SERVER: BuildTrack started"));
		if(FindGround())
		{
			UE_LOG(LogTemp,Warning,TEXT("SERVER: BuildTrack in progress"));
			Ground->SetTrackPoints(TrackPoints);
		}
	}
}

void ARaceGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARaceGameState, TrackPoints);
	DOREPLIFETIME(ARaceGameState, Leader);
	DOREPLIFETIME(ARaceGameState, Leaderboard);
}

void ARaceGameState::UpdateTimeTable(AKartPlayerState* PlayerState)
{
	if(!PlayerState) return;
	const int32 PlayerStateScore = FMath::Floor<int32>(PlayerState->GetScore());
	if(Leader == nullptr)
	{
		Leader = PlayerState;
		LeadingScore = PlayerStateScore;
		Leaderboard.Add(PlayerState);
	}
	else if(LeadingScore < PlayerStateScore)
	{
		Leader = PlayerState;
		LeadingScore = PlayerStateScore;
		if(Leaderboard[0] != PlayerState)
		{
			Leaderboard.Remove(PlayerState);
            Leaderboard.Insert(PlayerState, 0);
		}
	}
	else
	{
		for(auto It = Leaderboard.CreateConstIterator(); It; ++It)
		{
			const AKartPlayerState* OtherPlayerState = Cast<AKartPlayerState>(*It);
			if(OtherPlayerState)
			{
				if(PlayerState == OtherPlayerState)
				{
					// In right position already, we just invalidate the iterator.
					It.SetToEnd();
				}
				else if(OtherPlayerState->GetScore() < PlayerStateScore)
                {
					const int32 NewPosition = It.GetIndex();
					It.SetToEnd();
                	Leaderboard.Remove(PlayerState);
                	Leaderboard.Insert(PlayerState, NewPosition);
                }
				else if(It.GetIndex() == Leaderboard.Num() - 1)
                {
                    Leaderboard.AddUnique(PlayerState);
                }
			}
		}
		
	}
}

void ARaceGameState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Constructed GAMESTATE"));
}

void ARaceGameState::OnRep_TrackPoints()
{
	UE_LOG(LogTemp,Warning,TEXT("BuildTrack started"));
	if(FindGround())
	{
		UE_LOG(LogTemp,Warning,TEXT("BuildTrack in progress"));
		Ground->SetTrackPoints(TrackPoints);
	}
}

bool ARaceGameState::FindGround()
{
	if(Ground != nullptr) return true;
	AActor* GroundCandidate = UGameplayStatics::GetActorOfClass(this, AGround::StaticClass());
    if(GroundCandidate)
    {
    	Ground = Cast<AGround>(GroundCandidate);
    	return Ground != nullptr;
    }
	return false;
}
