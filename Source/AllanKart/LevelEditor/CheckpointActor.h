// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointActor.generated.h"

class ARaceGameMode;

UENUM(BlueprintType)
enum class ECheckpointType : uint8 {
	ECT_Start UMETA(DisplayName = "Start line"),
	ECT_StartAndFinish UMETA(DisplayName = "Start and finish line"),
	ECT_Sector UMETA(DisplayName = "Sector"),
	ECT_MicroSector UMETA(DisplayName = "Micro-sector"),
	ECT_Finish UMETA(DisplayName = "Finish line"),
	ECT_MAX UMETA(DisplayName = "Default MAX")
};

UCLASS()
class ALLANKART_API ACheckpointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACheckpointActor();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	ECheckpointType CheckpointType{ ECheckpointType::ECT_Sector };
	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OverlapWithCar(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	int32 CheckpointNumber{ -1 };
	int32 NumberOfCheckpoints{ -1 };
	int32 NumberOfLaps{ -1 };

public:	
	FORCEINLINE void SetCheckpointNumber(int32 Number) { CheckpointNumber = (CheckpointNumber == -1) ? Number : CheckpointNumber; }
	FORCEINLINE int32 GetCheckpointNumber() const { return CheckpointNumber; }
	FORCEINLINE void SetNumberOfCheckpoints(int32 Number) { NumberOfCheckpoints = (NumberOfCheckpoints == -1) ? Number : NumberOfCheckpoints; }
	FORCEINLINE void SetNumberOfLaps(int32 Number) { NumberOfLaps = (NumberOfLaps == -1) ? Number : NumberOfLaps; }
};
