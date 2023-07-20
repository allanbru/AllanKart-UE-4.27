// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceGameMode.h"

#include "AllanKart/Cars/Kart.h"
#include "AllanKart/GameState/RaceGameState.h"
#include "AllanKart/LevelEditor/LevelSaveGame.h"
#include "AllanKart/PlayerController/KartPlayerController.h"
#include "AllanKart/PlayerState/KartPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ARaceGameMode::ARaceGameMode()
{
	//Delay start
	bDelayedStart = true;
}

void ARaceGameMode::PassedCheckpoint(AKartPlayerState* PlayerState)
{
	RaceGameState = (RaceGameState == nullptr) ? GetGameState<ARaceGameState>() : RaceGameState;
	RaceGameState->UpdateTimeTable(PlayerState);
}

void ARaceGameMode::BeginPlay()
{
	Super::BeginPlay();

	ServerStartTime = GetWorld()->GetTimeSeconds();
	
	AActor* GroundCandidate = UGameplayStatics::GetActorOfClass(this, AGround::StaticClass());
	if(GroundCandidate)
	{
		Ground = Cast<AGround>(GroundCandidate);
		if(!Ground) return;
	}
	
	USaveGame* SavedMap = UGameplayStatics::LoadGameFromSlot(TrackTempFileName, 0);
	if(!SavedMap) return;
	
	const ULevelSaveGame* SavedLevel = Cast<ULevelSaveGame>(SavedMap);
	if(!SavedLevel) return;
	
	// Create track
	//if(!Ground->SetSplinePoints(SavedLevel->TrackPoints)) return;
	RaceGameState = (RaceGameState == nullptr) ? GetGameState<ARaceGameState>() : RaceGameState;
	if(RaceGameState)
	{
		RaceGameState->BuildTrack(SavedLevel->TrackPoints);
	}
	NumberOfCheckpoints = SavedLevel->TrackPoints.PointLocations.Num();

	// Set timer
	GetWorldTimerManager().SetTimer(StartTimerHandle, this, &ARaceGameMode::StartRace, StartDelay, false);
}

void ARaceGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void ARaceGameMode::StartRace()
{
	if (!Ground) return;
	TArray<AActor*> AllStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllStarts);
	for(AActor* Start : AllStarts)
	{
		if(Start) Start->Destroy();
	}
	
	// Create AI to fill players until DesiredNumberOfPlayers
	// Also, place karts at their positions starting from AI
	const int32 NumberOfPlayers = GetNumPlayers();
	
	for(int32 Index = 0; Index < NumberOfPlayers; Index++)
	{
		AController* PlayerController = UGameplayStatics::GetPlayerController(this, Index);
		if(PlayerController)
		{
			FTransform GridSlot = Ground->GetGridTransformAtPosition(DesiredNumberOfPlayers - NumberOfPlayers + Index);
			AKartPlayerController* KartPlayerController = Cast<AKartPlayerController>(PlayerController);
			if(KartPlayerController)
			{
				APlayerStart* NewStart = GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), GridSlot);
				if(NewStart)
				{
					NewStart->SetReplicates(true);
					ChoosePlayerStart(KartPlayerController);
				}
				KartPlayerController->SetGridSlot(GridSlot);
				KartPlayerController->SetInitialLocationAndRotation(GridSlot.GetLocation(), GridSlot.GetRotation().Rotator());
				AKartPlayerState* KartPlayerState = KartPlayerController->GetPlayerState<AKartPlayerState>();
				if(KartPlayerState)
				{
					KartPlayerState->SetScore(0);
					KartPlayerState->MulticastSetCheckpointsInLap(NumberOfCheckpoints);
				}
			}
		}
	}

	if(NumberOfPlayers < DesiredNumberOfPlayers && AIPawnClass)
	{
		const int32 AIPlayers = DesiredNumberOfPlayers - NumberOfPlayers;
		for(int32 Index=0; Index<AIPlayers; Index++)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FTransform SpawnTransform = Ground->GetGridTransformAtPosition(Index);
			FVector StartOffset = FVector::ZAxisVector * 200.f;
			SpawnTransform.SetLocation(SpawnTransform.GetLocation() + StartOffset);
			APawn* AIPlayer = GetWorld()->SpawnActor<APawn>(
				AIPawnClass,
				SpawnTransform,
				SpawnParameters
			);
			if(AIPlayer)
			{
				AKart* AIKart = Cast<AKart>(AIPlayer);
				if(AIKart)
				{
					AIKart->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
					AIKart->SetCanAffectNavigationGeneration(false, true);
					if(AIKart->GetPlayerState<AKartPlayerState>())
					{
						AKartPlayerState* KartPlayerState = AIKart->GetPlayerState<AKartPlayerState>();
						if(KartPlayerState)
						{
							KartPlayerState->SetIsABot(true);
							KartPlayerState->SetScore(0);
							KartPlayerState->MulticastSetCheckpointsInLap(NumberOfCheckpoints);
						}
					}
				}
			}
		}
	}
	
	// Start
	StartMatch();
}

void ARaceGameMode::FinishedRace(AKartPlayerState* Player, float TotalTime)
{
	if(Player)
	{
		FString PlayerName;
		Player->GetName(PlayerName);
		const int32 Minutes = FMath::Floor(TotalTime / 60);
		const int32 Seconds = FMath::Floor(TotalTime - Minutes * 60);
		const int32 Milliseconds = FMath::Floor(1000 * (TotalTime - FMath::Floor(TotalTime)));
		UE_LOG(LogTemp, Warning, TEXT("Race Finished for %s! RaceTime: %d:%2d:%3d"),
			*PlayerName, Minutes, Seconds, Milliseconds);
	}
}