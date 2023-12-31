// Copyright Epic Games, Inc. All Rights Reserved.


#include "ValidationGameMode.h"

#include "AllanKart/GameState/RaceGameState.h"
#include "AllanKart/LevelEditor/Ground.h"
#include "AllanKart/LevelEditor/LevelSaveGame.h"
#include "AllanKart/PlayerController/KartPlayerController.h"
#include "AllanKart/PlayerState/KartPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

AValidationGameMode::AValidationGameMode()
{
    bDelayedStart = true;
}

void AValidationGameMode::BeginPlay()
{
    Super::BeginPlay();

    AActor* GroundCandidate = UGameplayStatics::GetActorOfClass(this, AGround::StaticClass());
    if(GroundCandidate)
    {
    	Ground = Cast<AGround>(GroundCandidate);
    	if(!Ground) return;
    }

    USaveGame* SavedMap = UGameplayStatics::LoadGameFromSlot(TEXT("__PlayingMap__"), 0);
    if(!SavedMap) return;

    const ULevelSaveGame* SavedLevel = Cast<ULevelSaveGame>(SavedMap);
    if(!SavedLevel) return;

    // Create track
    if(!Ground->SetSplinePoints(SavedLevel->TrackPoints)) return;

    // Set timer
    GetWorldTimerManager().SetTimer(StartTimerHandle, this, &AValidationGameMode::StartRace, StartDelay, false);
}

void AValidationGameMode::StartRace()
{
    if(!Ground) return;

    // Delete all PlayerStarts

    TArray<AActor*> AllStarts;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllStarts);
    for(AActor* Start : AllStarts)
    {
        if(Start) Start->Destroy();
    }

    // Create PlayerStarts
    AController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if(PlayerController)
    {
        const FTransform GridSlot = Ground->GetGridTransformAtPosition(0);
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
        }
    }

	// Bind Events


	AGameMode::StartMatch();
}

void AValidationGameMode::PassedCheckpoint_Implementation(AKartPlayerState* Player)
{
    ARaceGameState* RaceGameState = GetGameState<ARaceGameState>();
    if(RaceGameState) RaceGameState->UpdateTimeTable(Player);
}

void AValidationGameMode::FinishedRace_Implementation(AKartPlayerState* Player, float TotalTime)
{
	float PlayerSpeed = -1.f;
	if(Player)
	{
	    const APawn* Pawn = Player->GetPawn();
	    if(!Pawn) return;
		PlayerSpeed = Pawn->GetVelocity().Size();
	}
	UE_LOG(LogTemp, Warning, TEXT("Player finished the race in %.2f, Speed: %.2f"), TotalTime, PlayerSpeed);
}
