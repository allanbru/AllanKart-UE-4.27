// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/PlayerState.h"
#include "MultiplayerSessionsSubsystem.h"


// Sets default values
ALobbyGameMode::ALobbyGameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ALobbyGameMode::RequestGameStart(const FString& Path)
{
	if(HasAuthority())
	{
		GetWorld()->ServerTravel(FString::Printf(TEXT("%s?listen"), *Path));
	}
}

// Called when the game starts or when spawned
void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	const UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		const UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);
		if(NewPlayer && NewPlayer->GetPlayerState<APlayerState>()) OnPlayerJoin.Broadcast(NewPlayer->GetPlayerState<APlayerState>());
	}
	
}

void ALobbyGameMode::PlayerLeft(APlayerState* LeavingPlayer)
{
	if(LeavingPlayer)
	{
		OnPlayerLeave.Broadcast(LeavingPlayer);
	}
}

// Called every frame
void ALobbyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

