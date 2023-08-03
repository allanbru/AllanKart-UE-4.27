// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckpointActor.h"
#include "Components/BoxComponent.h"
#include "AllanKart/Cars/Kart.h"

// Sets default values
ACheckpointActor::ACheckpointActor()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Checkpoint Trigger Box"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetHiddenInGame(true);

	SetRootComponent(TriggerBox);
}

void ACheckpointActor::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpointActor::OverlapWithCar);
}

void ACheckpointActor::OverlapWithCar(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AKart* Player = Cast<AKart>(OtherActor);
		if (Player)
		{
			Player->SetCurrentCheckpoint(CheckpointNumber, CheckpointType);
			if (NumberOfCheckpoints != -1 && Player->GetNumberOfCheckpoints() == -1)
			{
				Player->SetNumberOfCheckpoints(NumberOfCheckpoints);
				Player->SetNumberOfLaps(NumberOfLaps);
			}
		}
	}
}

