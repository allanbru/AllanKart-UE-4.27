// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoin, APlayerState*, NewPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeave, APlayerState*, NewPlayer);

UCLASS()
class ALLANKART_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALobbyGameMode();

	FOnPlayerJoin OnPlayerJoin;
	FOnPlayerLeave OnPlayerLeave;

	void RequestGameStart(const FString& Path);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	void PlayerLeft(APlayerState* LeavingPlayer);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
