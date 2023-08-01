// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyMenu.h"

#include "LobbyGameMode.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ULobbyMenu::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULobbyMenu, Players);
}

bool ULobbyMenu::Initialize()
{
	if(!Super::Initialize()) return false;
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
			if(GetWorld()->GetAuthGameMode())
			{
             	if(StartButton) StartButton->SetIsEnabled(true);
				ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
				if(LobbyGameMode)
				{
					LobbyGameMode->OnPlayerJoin.AddDynamic(this, &ThisClass::NewPlayerJoined);
				}
				APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
				if(PlayerState)
				{
					Players.Add(PlayerState);
					MulticastUpdatePlayersList();
				}
			}
			else
			{
				if(StartButton) StartButton->SetIsEnabled(false);
			}
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnSessionDestroyed);
		LobbyID = MultiplayerSessionsSubsystem->DesiredMatchType;
		if(RoomID)
		{
			RoomID->SetText(FText::FromString(LobbyID));
		}
	}

	if(StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &ThisClass::StartButtonClicked);
		MainMenuButton->OnClicked.AddDynamic(this, &ThisClass::MainMenuButtonClicked);
	}

	return true;
}

void ULobbyMenu::NewPlayerJoined(APlayerState* NewPlayer)
{
	if(NewPlayer)
	{
		Players.AddUnique(NewPlayer);
		MulticastUpdatePlayersList();
	}

}

void ULobbyMenu::PlayerLeft(APlayerState* LeavingPlayer)
{
	if(LeavingPlayer)
	{
		Players.Remove(LeavingPlayer);
	}
	MulticastUpdatePlayersList();
}

void ULobbyMenu::MulticastUpdatePlayersList_Implementation()
{
	FString NewList;
	for(const APlayerState* PlayerState : Players)
	{
		if(PlayerState)
		{
			NewList.Append(FString::Printf(TEXT("%s\n"), *PlayerState->GetPlayerName()));
		}
	}
	PlayersList->SetText(FText::FromString(NewList));
}

void ULobbyMenu::StartButtonClicked()
{
	if(GetWorld()->GetAuthGameMode())
	{
		ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
		if(LobbyGameMode)
		{
			LobbyGameMode->RequestGameStart(PathToMap);
			if(StartButton) StartButton->SetIsEnabled(false);
		}
	}
}

void ULobbyMenu::MainMenuButtonClicked()
{
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController)
	{
		if(MultiplayerSessionsSubsystem && PlayerController->IsLocalController() && PlayerController->HasAuthority())
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
		else
		{
			MenuTearDown();
            PlayerController->ClientTravel(PathToMainMenu, ETravelType::TRAVEL_Absolute);
		}

	}
}

void ULobbyMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

void ULobbyMenu::OnSessionDestroyed(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		MenuTearDown();
		if(GetGameInstance()->GetFirstLocalPlayerController()) GetGameInstance()->GetFirstLocalPlayerController()->ClientTravel(PathToMainMenu, TRAVEL_Absolute);
	}
}
