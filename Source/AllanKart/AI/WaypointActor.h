// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointActor.generated.h"

UCLASS()
class ALLANKART_API AWaypointActor : public AActor
{
	GENERATED_BODY()

public:

	AWaypointActor();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleInstanceOnly)
	int32 WaypointSequenceNumber{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaypointSpeed{1500.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AcceptanceRadius{300.f};

	
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnWaypointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* RewardSphere;
	
public:
	
	void SetWaypointSpeed(const float Speed);
	void SetWaypointData(const float Speed, const float DeviationDistance);
	
};
