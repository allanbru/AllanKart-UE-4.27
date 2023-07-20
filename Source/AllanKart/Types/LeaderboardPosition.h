#pragma once
#include "CoreMinimal.h"
#include "AllanKart/PlayerState/KartPlayerState.h"

#include "LeaderboardPosition.generated.h"

USTRUCT(BlueprintType)
struct FLeaderBoardPosition
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AKartPlayerState* PlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Position;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 NumberOfLaps;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CheckpointNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RaceTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DifferenceAhead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DifferenceToLeader;

	

	bool operator<(const FLeaderBoardPosition& B) const
	{
		if(PlayerState == nullptr) return true;
		if(B.PlayerState == nullptr) return false;
		// Score: 1st criteria
		if(PlayerState->GetScore() != B.PlayerState->GetScore()) return PlayerState->GetScore() < B.PlayerState->GetScore();

		// Race time: 2rd criteria
		return RaceTime < B.RaceTime;		
	}
	
	bool operator==(const FLeaderBoardPosition& B) const
	{
		return NumberOfLaps == B.NumberOfLaps && CheckpointNumber == B.CheckpointNumber && RaceTime == B.RaceTime;
	}
	
};
