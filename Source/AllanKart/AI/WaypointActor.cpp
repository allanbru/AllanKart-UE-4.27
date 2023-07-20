// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointActor.h"

#include "AIKartController.h"
#include "Components/SphereComponent.h"

AWaypointActor::AWaypointActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RewardSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RewardSphere"));
	RewardSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RewardSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	RewardSphere->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
	RewardSphere->SetGenerateOverlapEvents(true);
	RewardSphere->SetSphereRadius(AcceptanceRadius);
}

void AWaypointActor::BeginPlay()
{
	Super::BeginPlay();

	RewardSphere->OnComponentBeginOverlap.AddDynamic(this, &AWaypointActor::OnWaypointBeginOverlap);
}

void AWaypointActor::OnWaypointBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		const APawn* Player = Cast<APawn>(OtherActor);
		if(Player && Player->IsBotControlled() && Player->GetController())
		{
			AAIKartController* AIKartController = Cast<AAIKartController>(Player->GetController());
			if(AIKartController)
			{
				AIKartController->NotifyWaypointIntersection(WaypointSequenceNumber);
			}
		}
	}
}

void AWaypointActor::SetWaypointSpeed(const float Speed)
{
	WaypointSpeed = Speed;
}

void AWaypointActor::SetWaypointData(const float Speed, const float DeviationDistance)
{
	WaypointSpeed = Speed;
	AcceptanceRadius = DeviationDistance;
}

// Called every frame
void AWaypointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

