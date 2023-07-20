// Copyright Epic Games, Inc. All Rights Reserved.


#include "ValidationGameMode.h"
#include "AllanKart/Cars/Kart.h"
#include "AllanKart/LevelEditor/Ground.h"
#include "AllanKart/LevelEditor/LevelSaveGame.h"
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
	// Bind Events
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if(PlayerPawn)
	{
		AKart* PlayerKart = Cast<AKart>(PlayerPawn);
		if(PlayerKart)
		{
			PlayerKart->SetCanAffectNavigationGeneration(false, true);
			const FTransform GridSlot = Ground->GetGridTransformAtPosition(0);
			PlayerKart->SetActorTransform(GridSlot);
		}
	}

	AGameMode::StartMatch();	
}

void AValidationGameMode::PassedCheckpoint(AKart* Player, int32 Lap, int32 Checkpoint, float TotalTime)
{
	float PlayerSpeed = -1.f;
	if(Player)
	{
		PlayerSpeed = Player->GetVelocity().Size();
	}
	UE_LOG(LogTemp, Warning, TEXT("Checkpoint %d, Lap %d, Speed: %.2f"), Lap, Checkpoint, PlayerSpeed);
}

void AValidationGameMode::FinishedRace(AKart* Player, float TotalTime)
{
	float PlayerSpeed = -1.f;
	if(Player)
	{
		PlayerSpeed = Player->GetVelocity().Size();
	}
	UE_LOG(LogTemp, Warning, TEXT("Player finished the race in %.2f, Speed: %.2f"), TotalTime, PlayerSpeed);
}
