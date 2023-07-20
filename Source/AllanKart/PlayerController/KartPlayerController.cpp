// Fill out your copyright notice in the Description page of Project Settings.


#include "KartPlayerController.h"

#include "AllanKart/Cars/Kart.h"
#include "AllanKart/GameMode/RaceGameMode.h"
#include "AllanKart/GameState/RaceGameState.h"
#include "AllanKart/HUD/KartHUD.h"
#include "AllanKart/HUD/KartOverlay.h"
#include "AllanKart/LevelEditor/Ground.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void AKartPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetServerTime();
	SetInputMode(FInputModeGameAndUI());
}

void AKartPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollInit();
	CheckTimeSync(DeltaSeconds);
}

void AKartPlayerController::PollInit()
{

	if (GameMode == nullptr && GetWorld()->GetAuthGameMode())
	{
		GameMode = GetWorld()->GetAuthGameMode();
	}

	if (KartHUD == nullptr && GetHUD())
	{
		KartHUD = Cast<AKartHUD>(GetHUD());
	}

	if(KartPlayerState == nullptr && GetPlayerState<AKartPlayerState>())
	{
		KartPlayerState = GetPlayerState<AKartPlayerState>();
		KartPlayerState->SetIsABot(false);
	}
}

void AKartPlayerController::CheckTimeSync(const float DeltaSeconds)
{
	TimeSyncRunningTime += DeltaSeconds;
	if (IsLocalController() && TimeSyncRunningTime >= TimeSyncDeltaTime)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AKartPlayerController::SetHUDTimer_Implementation(float Time)
{
	if (KartHUD && KartHUD->KartOverlay && KartHUD->KartOverlay->Timer)
	{
		const int32 Minutes = FMath::Floor<int32>(Time / 60.f);
		const int32 Seconds = FMath::Floor<int32>(Time - 60.f * Minutes);
		const int32 Milliseconds = FMath::Floor<int32>((Time - 60.f * Minutes - Seconds) * 1000.f);
		const FText TimerText = FText::FromString(FString::Printf(TEXT("%02d:%02d:%03d"), Minutes, Seconds, Milliseconds));
		KartHUD->KartOverlay->Timer->SetText(TimerText);
		GetWorldTimerManager().ClearTimer(ChronometerTimer);
		GetWorldTimerManager().SetTimer(ChronometerTimer, this, &AKartPlayerController::ClearTimer, 2.f, false);
	}
}

void AKartPlayerController::SetHUDFastestLap_Implementation(float Time)
{
	if (KartHUD && KartHUD->KartOverlay && KartHUD->KartOverlay->FastestLap)
	{
		const int32 Minutes = FMath::Floor<int32>(Time / 60.f);
		const int32 Seconds = FMath::Floor<int32>(Time - 60.f * Minutes);
		const int32 Milliseconds = FMath::Floor<int32>((Time - 60.f * Minutes - Seconds) * 1000.f);
		const FText TimerText = FText::FromString(FString::Printf(TEXT("%02d:%02d:%03d"), Minutes, Seconds, Milliseconds));
		KartHUD->KartOverlay->FastestLap->SetText(TimerText);
	}
}

void AKartPlayerController::SetHUDSpeed(float Speed)
{
	if (KartHUD && KartHUD->KartOverlay && KartHUD->KartOverlay->Speed)
	{
		const int32 SpeedToPrint = FMath::Abs(FMath::Floor(Speed));
		const FText SpeedText = FText::FromString(FString::Printf(TEXT("%03d"), SpeedToPrint));
		KartHUD->KartOverlay->Speed->SetText(SpeedText);
	}
}

void AKartPlayerController::SetHUDGear(int32 Gear)
{
	if (KartHUD && KartHUD->KartOverlay && KartHUD->KartOverlay->Gear)
	{
		FText GearText = FText::FromString(FString::Printf(TEXT("%d"), Gear));
		if (Gear == 0)
		{
			GearText = FText::FromString(TEXT("N"));
		}
		else if (Gear < 0)
		{
			GearText = FText::FromString(TEXT("R"));
		}
		KartHUD->KartOverlay->Gear->SetText(GearText);
	}
}

void AKartPlayerController::SetHUDLaps_Implementation(int32 Laps)
{
	if (KartHUD && KartHUD->KartOverlay && KartHUD->KartOverlay->Laps)
	{
		FText LapsText = FText::FromString(FString::Printf(TEXT("%d"), Laps));
		KartHUD->KartOverlay->Laps->SetText(LapsText);
	}
}

void AKartPlayerController::SetHUDRevs(float Revs, float MaxRevs)
{
	if (KartHUD && KartHUD->KartOverlay && KartHUD->KartOverlay->EngineRev)
	{
		float Percentage = FMath::Clamp(Revs / MaxRevs, 0.f, 1.f);
		KartHUD->KartOverlay->EngineRev->SetPercent(Percentage);
		KartHUD->KartOverlay->EngineRev->SetFillColorAndOpacity(FLinearColor::LerpUsingHSV(FLinearColor::Blue, FLinearColor::Red, Percentage));
	}
}

void AKartPlayerController::SetHUDPosition_Implementation(const int32 PositionInScreen, const int32 Position, const FString& PlayerName, const float TimeDiff)
{
	if(KartHUD && KartHUD->KartOverlay && PositionInScreen < KartHUD->KartOverlay->PositionOverlay.Num() && KartHUD->KartOverlay->PositionOverlay[PositionInScreen] != nullptr)
	{
		UPositionOverlay* WidgetToChange = KartHUD->KartOverlay->PositionOverlay[PositionInScreen];
		if(WidgetToChange && WidgetToChange->DriverPosition && WidgetToChange->DriverName && WidgetToChange->DriverDifference)
		{
			WidgetToChange->SetVisibility(ESlateVisibility::Visible);
			const FText PositionText = FText::FromString(FString::Printf(TEXT("%d"), Position+1));
			WidgetToChange->DriverPosition->SetText(PositionText);
			
			WidgetToChange->DriverName->SetText(FText::FromString(PlayerName));

			const int32 Seconds = FMath::FloorToInt(TimeDiff);
			const int32 Milliseconds = FMath::FloorToInt(1000.f * (TimeDiff - Seconds));
			const FText TimeDiffString = FText::FromString(FString::Printf(TEXT("%02d:%03d"), Seconds, Milliseconds));
			WidgetToChange->DriverDifference->SetText(TimeDiffString);
			if(KartHUD->KartOverlay->PositionsList)
			{
				if(PositionInScreen == 0) KartHUD->KartOverlay->PositionsList->ClearListItems();
				KartHUD->KartOverlay->PositionsList->AddItem(WidgetToChange);
			}
		}
	}
}

void AKartPlayerController::UpdateLeaderboard()
{
	if(FindGameMode() && FindGameState() && KartPlayerState)
	{
		RaceGameMode->PassedCheckpoint(KartPlayerState);
		const int32 NumPlayers = RaceGameState->Leaderboard.Num();
		const int32 Position = RaceGameState->Leaderboard.Find(KartPlayerState);
		if(Position < NumPlayers && RaceGameState->Leader != nullptr && KartHUD && KartHUD->KartOverlay)
		{
			const int32 AppearInLeaderboard = KartHUD->KartOverlay->PositionsToShow;
			int32 PositionsAhead = FMath::Floor(.5 * (AppearInLeaderboard - 1));
			int32 PositionsBehind = PositionsAhead;
			if(Position - PositionsAhead < 0)
			{
				PositionsAhead = Position;
				PositionsBehind = FMath::Clamp(2*PositionsBehind - Position, 0, NumPlayers - 1 - Position);
			}
			else if(Position + PositionsBehind >= NumPlayers)
			{
				PositionsBehind = NumPlayers - 1 - Position;
				PositionsAhead = FMath::Clamp(2*PositionsAhead - Position, 0, NumPlayers - 1 - Position);
			}
			for(int32 i = (Position - PositionsAhead); i < NumPlayers && i <= (Position + PositionsBehind); i++)
			{
				if(i >= 0)
				{
					AKartPlayerState* OtherPlayer = RaceGameState->Leaderboard[i];
                    if(RaceGameState->Leaderboard[i] != nullptr)
                    {
                    	const int32 OtherPlayerScore = OtherPlayer->GetScore();
                    	const float TimeDiff = OtherPlayer->GetRaceTimeInScore(OtherPlayerScore) - RaceGameState->Leader->GetRaceTimeInScore(OtherPlayerScore);
                    	SetHUDPosition(i, Position, OtherPlayer->GetPlayerName(), TimeDiff);
                    }
				}
			}
		}
	}
}

void AKartPlayerController::ResetToLastCheckpoint()
{
	ServerResetToLastCheckpoint();
}

void AKartPlayerController::ServerResetToLastCheckpoint_Implementation()
{
	MulticastResetToLastCheckpoint();
}

void AKartPlayerController::MulticastResetToLastCheckpoint_Implementation()
{
	GameMode = (GameMode == nullptr) ? GetWorld()->GetAuthGameMode() : GameMode;
	if(GetPawn() == nullptr || GameMode == nullptr) return;
	PlayerKart = (PlayerKart == nullptr) ? Cast<AKart>(GetPawn()) : PlayerKart;
	if(PlayerKart && FindGround())
	{
		const int32 CurrentCheckpoint = PlayerKart->GetCheckpointNumber();
		FTransform CheckpointTransform = Ground->GetCheckpointTransform(CurrentCheckpoint);
		const FVector Offset = FVector::ZAxisVector * 300.f;
		CheckpointTransform.SetLocation(CheckpointTransform.GetLocation() + Offset);
		GameMode->RestartPlayerAtTransform(this, CheckpointTransform);
		PlayerKart->SetActorTransform(CheckpointTransform, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AKartPlayerController::ServerRequestServerTime_Implementation(const float TimeOfClientRequest)
{
	const float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AKartPlayerController::ClientReportServerTime_Implementation(const float TimeOfClientRequest, const float TimeServerReceivedClientRequest)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = .5f * RoundTripTime;
	const float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AKartPlayerController::GetServerTime() const
{
	if(HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() - SingleTripTime;
}

bool AKartPlayerController::FindGround()
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

void AKartPlayerController::SetGridSlot(const FTransform& NewGridSlot)
{
	GridSlot = NewGridSlot;
}

bool AKartPlayerController::FindGameState()
{
	if(RaceGameState) return true;
	AGameStateBase* Candidate = UGameplayStatics::GetGameState(this);
	if(Candidate)
	{
		RaceGameState = Cast<ARaceGameState>(Candidate);
		return RaceGameState != nullptr;
	}
	return false;
}

bool AKartPlayerController::FindGameMode()
{
	GameMode = (GameMode == nullptr) ? GetWorld()->GetAuthGameMode() : GameMode;
	if(GameMode)
	{
		RaceGameMode = (RaceGameMode == nullptr) ? Cast<ARaceGameMode>(GameMode) : RaceGameMode;
		return RaceGameMode != nullptr;
	}
	return false;
}

void AKartPlayerController::ClearTimer()
{
	if (KartHUD && KartHUD->KartOverlay && KartHUD->KartOverlay->Timer)
	{
		const FText TimerText = FText::FromString("");
		KartHUD->KartOverlay->Timer->SetText(TimerText);
	}
}
