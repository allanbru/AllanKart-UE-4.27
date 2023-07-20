// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AllanKart/Types/TrackPoints.h"
#include "GameFramework/GameState.h"
#include "RaceGameState.generated.h"

class AKartPlayerState;
/**
 * 
 */
class AGround;
UCLASS()
class ALLANKART_API ARaceGameState : public AGameState
{
	GENERATED_BODY()

public:
	ARaceGameState();

	UFUNCTION()
	void BuildTrack(const FTrackPoints& NewTrackPoints);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	void UpdateTimeTable(AKartPlayerState* PlayerState);

	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<AKartPlayerState*> Leaderboard;

	UPROPERTY(Replicated)
    AKartPlayerState* Leader;
	
protected:

	virtual void BeginPlay() override;
	
private:
	
	UPROPERTY(ReplicatedUsing=OnRep_TrackPoints)
	FTrackPoints TrackPoints;
		
	UFUNCTION()
	void OnRep_TrackPoints();
	
	UPROPERTY()
	AGround* Ground;

	bool FindGround();	
	
	UPROPERTY()
	int32 LeadingScore{-1};

public:
	
	
};
