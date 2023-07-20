// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyMenu.generated.h"

/**
 * 
 */
class UButton;
class UTextBlock;
class UEditableTextBox;
UCLASS()
class ALLANKART_API ULobbyMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayersList;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* RoomID;

	UPROPERTY(Replicated)
	TArray<APlayerState*> Players;

private:

	UPROPERTY(VisibleInstanceOnly);
	FString LobbyID;

	UFUNCTION()
	void NewPlayerJoined(APlayerState* NewPlayer);

	UFUNCTION()
	void PlayerLeft(APlayerState* LeavingPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdatePlayersList();
	
	UFUNCTION()
	void StartButtonClicked();

	UFUNCTION()
	void MainMenuButtonClicked();

	void MenuTearDown();

	UFUNCTION()
	void OnSessionDestroyed(bool bWasSuccessful);

	UPROPERTY()
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString PathToMainMenu{TEXT("/Game/Menu/MainMenu")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString PathToMap{TEXT("/Game/LevelEditor/Maps/MPPlayLevel")};
	
};
