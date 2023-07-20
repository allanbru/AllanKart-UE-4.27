// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LevelEditorPawn.generated.h"

UCLASS()
class ALLANKART_API ALevelEditorPawn : public APawn
{
	GENERATED_BODY()

public:
	ALevelEditorPawn();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* TopDownCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PreviewItem;

	UPROPERTY(BlueprintReadOnly)
	bool bHoldingMovePoint{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bTopDownCamera{ true };

	UPROPERTY(BlueprintReadOnly)
	int32 EditingIndex { -1 };

	UPROPERTY(BlueprintReadOnly)
	FVector ProjectedLocation { 0.f, 0.f, 0.f };

	UPROPERTY(BlueprintReadWrite)
	class ULevelSaveGame* SaveGameFile;

	FString SaveSlotName{ "AutoSave" };

	UPROPERTY(BlueprintReadOnly)
	class AGround* Ground;

protected:
	virtual void BeginPlay() override;

private:

	float AccumulatedDeltaTime{0.f};
	UPROPERTY(EditAnywhere,  Category = "Level Editor")
	float CheckMouseTime{.2f};

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	class UNavigationSystemBase* NavigationSystem;

	// Terrain Height edit step
	UPROPERTY(EditAnywhere, Category = "Level Editor")
	float HeightStep{50.f};

	// Snapping
	bool bSnapToMesh{false};
	
	// Helper functions

	void CalculateProjectedLocation();
	AGround* FindGround();
	APlayerController* FindController();
	UNavigationSystemBase* FindNavigationSystem();
	void GetSphereAtCursor();
	void HandleMovementInput();
	void MoveHeld();
	
	/**
	*** Input functions
	**/

	// Camera

	void SwitchCamera();

	// Editor

	void AddButtonPressed();
	void MoveButtonPressed();
	void MoveButtonReleased();
	void RemoveButtonPressed();
	void ToggleLoop();
	void IncreaseTerrainHeight();
	void DecreaseTerrainHeight();
	void RotateTangentInput(float Amount);

	void SnapToMesh();
	void DeSnapToMesh();
	//Helper
	void ChangeTerrainHeight(float Amount);

	// Movement

	void MoveX(float Amount);
	void MoveY(float Amount);
	void MoveZ(float Amount);
	void LookUp(float Amount);
	void LookRight(float Amount);

	// 3D Editor:

	// Items that can be placed into the world
	UPROPERTY(EditDefaultsOnly, Category = "Level Editor")
	TArray<class UStaticMesh*> PlaceableItems;
	
	TArray<FName> AllItems;
	FName SelectedItem;

	int32 SelectedMeshIndex{-1};

	bool bCanPlaceItem{ false };
	FVector PlacingPosition{ 0.f,0.f,0.f };
	FRotator PlacingRotation{0.f,0.f,0.f};
	void CalculatePlaceablePoint();


public:	
	

	

};
