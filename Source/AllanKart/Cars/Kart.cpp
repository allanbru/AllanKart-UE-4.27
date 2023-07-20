// Fill out your copyright notice in the Description page of Project Settings.


#include "Kart.h"
#include "AllanKart/AI/AIKartController.h"
#include "AllanKart/PlayerController/KartPlayerController.h"
#include "Animation/AnimMontage.h"
#include "WheeledVehicleMovementComponent.h"
#include "AllanKart/PlayerState/KartPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

AKart::AKart()
{
	SetReplicates(true);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->TargetArmLength = 500.f;
	SpringArmComponent->SocketOffset = FVector(0.f, 0.f, 200.f);
	SpringArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f).Quaternion());

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);

	DriverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DriverMesh"));
	DriverMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DriverMesh->SetupAttachment(GetRootComponent(), FName("SeatSocket"));

	EngineSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Engine Sound"));
	EngineSound->SetupAttachment(GetRootComponent(), FName("Exhaust"));
}

void AKart::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	
	SetCanAffectNavigationGeneration(false, true);
	if(GetVehicleMovementComponent())
	{
		GetMesh()->SetCanEverAffectNavigation(true);
		GetMesh()->bFillCollisionUnderneathForNavmesh = true;
	}
}

void AKart::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	PollInit();
	
	UpdateEngineSound();
	UpdateHUD();
}

void AKart::PollInit()
{
	if(KartPlayerState == nullptr)
	{
		KartPlayerState = GetPlayerState<AKartPlayerState>();
		if(KartPlayerState)
		{
			SetSpawnPoint();
		}
	}
}


void AKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Cheer"), EInputEvent::IE_Pressed, this, &AKart::PlayCelebrationMontage);

	PlayerInputComponent->BindAxis(TEXT("ThrottleAxis"), this, &AKart::ThrottleInput);
	PlayerInputComponent->BindAxis(TEXT("BrakeAxis"), this, &AKart::BrakeInput);
	PlayerInputComponent->BindAxis(TEXT("SteeringAxis"), this, &AKart::SteeringInput);
}

void AKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AKart::SetDriver(const FKartDriver& NewDriver)
{
	ServerSetDriver(NewDriver);
}

void AKart::ServerSetDriver_Implementation(const FKartDriver& NewDriver)
{
	MulticastSetDriver(NewDriver);
}

void AKart::MulticastSetDriver_Implementation(const FKartDriver& NewDriver)
{
	if (NewDriver.DriverMesh && NewDriver.DriverAnimInstance && CelebrationMontage)
	{
		DriverMesh->SetAnimInstanceClass(nullptr);
		DriverMesh->SetSkeletalMesh(NewDriver.DriverMesh);
		DriverMesh->SetRelativeTransform(NewDriver.DefaultTransform);
		DriverMesh->SetAnimInstanceClass(NewDriver.DriverAnimInstance);
		CelebrationMontage = NewDriver.DrivingMontage;
	}
}

void AKart::UpdateHUD_Implementation()
{
	if (!Controller || !GetVehicleMovementComponent()) return;
	KartController = (KartController == nullptr) ? Cast<AKartPlayerController>(Controller) : KartController;
	if (KartController)
	{
		KartController->SetHUDGear(GetVehicleMovementComponent()->GetCurrentGear());
		KartController->SetHUDSpeed(GetVehicleMovementComponent()->GetForwardSpeed() * 0.036f); //cm/s -> km/h
		KartController->SetHUDRevs(GetVehicleMovementComponent()->GetEngineRotationSpeed(), GetVehicleMovementComponent()->GetEngineMaxRotationSpeed());
	}
}

void AKart::ThrottleInput(float Throttle)
{
	if (GetVehicleMovementComponent())
	{
		GetVehicleMovementComponent()->SetThrottleInput(Throttle);
	}
}

void AKart::BrakeInput(float Brake)
{
	if (GetVehicleMovementComponent())
	{
		GetVehicleMovementComponent()->SetBrakeInput(Brake);
	}
}

void AKart::SteeringInput(float Steering)
{
	if (GetVehicleMovementComponent())
	{
		GetVehicleMovementComponent()->SetSteeringInput(Steering);
	}
}

void AKart::PlayCelebrationMontage_Implementation()
{
	MulticastPlayCelebrationMontage();	
}

void AKart::MulticastPlayCelebrationMontage_Implementation()
{
	if (DriverMesh && DriverMesh->GetAnimInstance() && !DriverMesh->GetAnimInstance()->IsAnyMontagePlaying())
	{
		bPlayingAnimation = true;
		if (CelebrationMontage && CelebrationMontage->GetSectionIndex(FName("Cheering")) != INDEX_NONE)
		{
			DriverMesh->GetAnimInstance()->Montage_Play(CelebrationMontage);
			DriverMesh->GetAnimInstance()->Montage_JumpToSection(FName("Cheering"));
		}
		bPlayingAnimation = false;
	}
}

void AKart::UpdateEngineSound()
{
	if (!GetVehicleMovementComponent()) return;
	if (EngineSound)
	{
		EngineSound->SetFloatParameter(FName("RPM"), GetVehicleMovementComponent()->GetEngineRotationSpeed());
	}
}

void AKart::SetSpawnPoint()
{
	KartController = (KartController == nullptr) ? Cast<AKartPlayerController>(Controller) : KartController;
	if(HasAuthority() && KartController)
	{
		SetActorTransform(KartController->GetGridSlot(), false, nullptr, ETeleportType::TeleportPhysics);
	}
	AIKartController = (AIKartController == nullptr) ? Cast<AAIKartController>(Controller) : AIKartController;
}

void AKart::SetCurrentCheckpoint(int32 Checkpoint, ECheckpointType CheckpointType)
{
	bool bIsABot = false;
	KartController = (KartController == nullptr) ? Cast<AKartPlayerController>(Controller) : KartController;
	if (!KartController)
	{
		AIKartController = (AIKartController == nullptr) ? Cast<AAIKartController>(Controller) : AIKartController;
		bIsABot = true;
		if(!AIKartController) return;
	}
	
	const float CurrentWorldTime = (bIsABot) ? GetWorld()->GetTimeSeconds() : KartController->GetServerTime();
	const float CurrentLapTime = CurrentWorldTime - TimeOffset;
	OnCheckpointPassed.Broadcast(this, CurrentLap, Checkpoint, CurrentWorldTime);
	
	if(!bIsABot) KartController->SetHUDTimer(CurrentLapTime);
	if (Checkpoint == CurrentCheckpoint + 1)
	{
		CurrentCheckpoint = Checkpoint;
	}
	
	if (CheckpointType == ECheckpointType::ECT_Finish)
	{
		// FinishCircuit
		if (CurrentLapTime < FastestLap || FastestLap == 0.f)
		{
			FastestLap = CurrentLapTime;
			if(!bIsABot) KartController->SetHUDFastestLap(CurrentLapTime);
			PlayCelebrationMontage();
		}
		OnRaceFinished.Broadcast(this, CurrentWorldTime);
	}

	if (CheckpointType == ECheckpointType::ECT_StartAndFinish && CurrentCheckpoint != -1 && CurrentCheckpoint == NumberOfCheckpoints - 1)
	{
		CurrentLap++;
		if(!bIsABot) KartController->SetHUDLaps(CurrentLap);
		if (CurrentLapTime < FastestLap || FastestLap == 0.f)
		{
			FastestLap = CurrentLapTime;
			if(!bIsABot) KartController->SetHUDFastestLap(CurrentLapTime);
			PlayCelebrationMontage();
		}
		TimeOffset = CurrentWorldTime;
		CurrentCheckpoint = Checkpoint;
		if (CurrentLap == NumberOfLaps)
		{
			OnRaceFinished.Broadcast(this, CurrentWorldTime);
		}
	}

	KartPlayerState = (KartPlayerState == nullptr) ? GetPlayerState<AKartPlayerState>() : KartPlayerState;
	if(KartPlayerState)
	{
		KartPlayerState->SetCurrentLap(CurrentLap);
		KartPlayerState->ServerSetCurrentCheckpoint(Checkpoint, CurrentWorldTime);
	}
}
