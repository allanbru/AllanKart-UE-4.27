// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelEditorPawn.h"
#include "AI/NavigationSystemBase.h"
#include "AllanKart/Inventory/InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Ground.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationData.h"
#include "PlacedMeshComponent.h"

ALevelEditorPawn::ALevelEditorPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	Capsule->SetSimulatePhysics(false);
	Capsule->SetEnableGravity(false);
	SetRootComponent(Capsule);

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	TopDownCamera->OrthoWidth = 10000.f;
	//TopDownCamera->AttachToComponent(Capsule, FAttachmentTransformRules::KeepWorldTransform);
	TopDownCamera->AddLocalOffset(FVector(0.f, 0.f, 15000.f));

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Capsule);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(SkeletalMesh);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	PreviewItem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewItem"));
	PreviewItem->SetVisibility(false);
	PreviewItem->SetupAttachment(SkeletalMesh);
	PreviewItem->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PreviewItem->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	PreviewItem->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	PreviewItem->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	PreviewItem->SetWorldRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));
	PreviewItem->SetSimulatePhysics(false);
	PreviewItem->SetEnableGravity(false);
}

void ALevelEditorPawn::BeginPlay()
{
	Super::BeginPlay();
	
	FirstPersonCamera->SetActive(false);
	TopDownCamera->SetActive(true);
	bTopDownCamera = true;

    // I have a mistake with the camera at the beggining.... dk why
    SwitchCamera();
    SwitchCamera();

	if(FindGround())
	{
		const auto& FoundItem = InventoryComponent->GetSelectedItem();

		//Debug
        if (FoundItem.Mesh)
        {
            PreviewItem->SetStaticMesh(FoundItem.Mesh);
        }	
	}
}

void ALevelEditorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//AccumulatedDeltaTime += DeltaTime;
	HandleMovementInput();
	
	if (bTopDownCamera)
	{
		if (bHoldingMovePoint)
		{
			MoveHeld();
		}
	}
	else
	{
		CalculatePlaceablePoint(); //Tem jeito melhor? Usando só a NavMesh talvez, aproveitando a colisão já calculada
	}
}

void ALevelEditorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("SwitchCamera"), EInputEvent::IE_Pressed, this, &ALevelEditorPawn::SwitchCamera);

	PlayerInputComponent->BindAction(TEXT("Editor_Add"), EInputEvent::IE_Pressed, this, &ALevelEditorPawn::AddButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Editor_Move"), EInputEvent::IE_Pressed, this, &ALevelEditorPawn::MoveButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Editor_Move"), EInputEvent::IE_Released, this, &ALevelEditorPawn::MoveButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Editor_Remove"), EInputEvent::IE_Pressed, this, &ALevelEditorPawn::RemoveButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Editor_ToggleLoop"), EInputEvent::IE_Pressed, this, &ALevelEditorPawn::ToggleLoop);
	PlayerInputComponent->BindAction(TEXT("Editor_IncreaseHeight"), EInputEvent::IE_Pressed, this, &ALevelEditorPawn::IncreaseTerrainHeight);
	PlayerInputComponent->BindAction(TEXT("Editor_DecreaseHeight"), EInputEvent::IE_Pressed, this, &ALevelEditorPawn::DecreaseTerrainHeight);
	PlayerInputComponent->BindAction(TEXT("Editor_SnapToMesh"), IE_Pressed, this, &ALevelEditorPawn::SnapToMesh);
	PlayerInputComponent->BindAction(TEXT("Editor_SnapToMesh"), IE_Released, this, &ALevelEditorPawn::DeSnapToMesh);
	
	PlayerInputComponent->BindAxis(TEXT("Editor_RotateTangent"), this, &ALevelEditorPawn::RotateTangentInput);
	
	PlayerInputComponent->BindAxis(TEXT("Editor_X"), this, &ALevelEditorPawn::MoveX);
	PlayerInputComponent->BindAxis(TEXT("Editor_Y"), this, &ALevelEditorPawn::MoveY);
	PlayerInputComponent->BindAxis(TEXT("Editor_Z"), this, &ALevelEditorPawn::MoveZ);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ALevelEditorPawn::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &ALevelEditorPawn::LookRight);
}

void ALevelEditorPawn::CalculateProjectedLocation()
{
	if (FindGround() && FindController() && FindNavigationSystem())
	{
		FVector MousePosition, MouseDirection;
		if (PlayerController->DeprojectMousePositionToWorld(MousePosition, MouseDirection))
		{
			FNavLocation OutLocation;
			const FVector Extent(0.f, 0.f, 50000.f);
			if (NavigationSystem->GetMainNavData() && NavigationSystem->GetMainNavData()->ProjectPoint(MousePosition, OutLocation, Extent))
			{
				ProjectedLocation = OutLocation.Location;
			}
			else
			{
				ProjectedLocation = FVector(0.f, 0.f, 0.f);
			}
		}
	}
}

void ALevelEditorPawn::CalculatePlaceablePoint()
{
	if (FindGround() && FindController() && FindNavigationSystem() && PreviewItem != nullptr)
	{
		int32 ScreenX, ScreenY;
		PlayerController->GetViewportSize(ScreenX, ScreenY);
		FVector MousePosition, MouseDirection;
		if (PlayerController->DeprojectScreenPositionToWorld(ScreenX / 2.f, ScreenY / 2.f, MousePosition, MouseDirection))
		{
			FHitResult HitResult;
			const FVector Extent(1000.f , 1000.f, 10000.f);
			GetWorld()->LineTraceSingleByChannel(HitResult, MousePosition, MousePosition + MouseDirection.GetSafeNormal() * 10000.f, ECollisionChannel::ECC_Visibility);
			if (HitResult.bBlockingHit)
			{
				FVector ImpactPoint = HitResult.ImpactPoint;
				FRotator CalculatedRotation = FRotator(0.f,GetActorRotation().Yaw - 90.f,0.f);
				
				if(HitResult.GetComponent())
				{
					FVector HitVector = HitResult.ImpactNormal;
					float PX = FVector::DotProduct(HitVector, FVector::XAxisVector);
					float PY = FVector::DotProduct(HitVector, FVector::YAxisVector);
					float PZ = FVector::DotProduct(HitVector, FVector::ZAxisVector);
					int32 HitDirection = 0;
					if(FMath::Abs(PX) > FMath::Abs(PY))
					{
						if(FMath::Abs(PX) > FMath::Abs(PZ))
						{
							HitDirection = 0;
						}
						else
						{
							HitDirection = 2;
						}
					}
					else
					{
						if(FMath::Abs(PY) > FMath::Abs(PZ))
						{
							HitDirection = 1;
						}
						else
						{
							HitDirection = 2;
						}
					}
					
					float SelectedP = 0.f;
					FVector SelectedDirection;
					
					switch(HitDirection)
					{
					case 0:
						SelectedP = PX;
						SelectedDirection = FMath::Sign(SelectedP) * FVector::XAxisVector;
						break;
					case 1:
						SelectedP = PY;
						SelectedDirection = FMath::Sign(SelectedP) * FVector::YAxisVector;
						break;
					case 2:
						SelectedP = PZ;
						SelectedDirection = FMath::Sign(SelectedP) * FVector::ZAxisVector;
						break;
					default:
						break;
					}
					
					ImpactPoint += SelectedDirection;
        /*
					if(bSnapToMesh && HitResult.GetComponent()->ComponentHasTag("StackableItem"))
					{
						UStaticMeshComponent* HitMesh = Cast<UStaticMeshComponent>(HitResult.GetComponent());
						if(HitMesh && HitMesh->GetStaticMesh())
						{
							FVector Center, Extents;
							HitMesh->GetLocalBounds().GetCenterAndExtents(Center, Extents);
							// Impact Point: Component Surface Center = Component Origin + Extents . SelectedDirection
							// Component Origin: Center - 1/2 Extents
							// Box Extent is wrong for some meshes....
							ImpactPoint = Center - .5f * Extents; // Origin
							ImpactPoint += FMath::Abs(Extents.Dot(SelectedDirection)) * SelectedDirection.GetSafeNormal(); //Other component origin should be here;
							ImpactPoint += .5f * PreviewItem->GetStreamingBounds().GetExtent() * FVector(1.f,1.f,0.f); //PreviewItem starts from Z = 0, so pushing item to start on (0,0,0)
							
							CalculatedRotation = HitMesh->GetComponentRotation();
						}
					}
        */
					if(HitResult.GetComponent()->ComponentHasTag("EditableItem"))
					{
						UPlacedMeshComponent* HitComponent = Cast<UPlacedMeshComponent>(HitResult.GetComponent());
						if(HitComponent)
						{
							SelectedMeshIndex = HitComponent->PlacedMeshIndex;
						}
					}
					else
					{
						SelectedMeshIndex = -1;
					}

					
				}
				bCanPlaceItem = true;
				FTransform NewTransform = InventoryComponent->GetSelectedItem().Transform;
				FTransform GoodTransform;
				GoodTransform.SetLocation(ImpactPoint);
				GoodTransform.SetRotation(CalculatedRotation.Quaternion() * PlacingRotation.Quaternion());
				GoodTransform.SetScale3D(FVector(1.f,1.f,1.f));
				GoodTransform.Accumulate(NewTransform);
				PreviewItem->SetWorldTransform(GoodTransform);
				PreviewItem->SetVisibility(true);
			}
			else
			{
				bCanPlaceItem = false;
				SelectedMeshIndex = -1;
				PreviewItem->SetVisibility(false);
			}
		}
	}
	else
	{
		bCanPlaceItem = false;
		SelectedMeshIndex = -1;
	}
}

AGround* ALevelEditorPawn::FindGround()
{
	if (Ground == nullptr)
	{
		AActor* PotentialGround = UGameplayStatics::GetActorOfClass(this, AGround::StaticClass());
		if (PotentialGround)
		{
			Ground = Cast<AGround>(PotentialGround);
			return Ground;
		}
		return nullptr;
	}
	return Ground;
}

APlayerController* ALevelEditorPawn::FindController()
{
	if (Controller && PlayerController == nullptr)
	{
		PlayerController = Cast<APlayerController>(Controller);
	}
	return PlayerController;
}

UNavigationSystemBase* ALevelEditorPawn::FindNavigationSystem()
{
	if (NavigationSystem == nullptr)
	{
		NavigationSystem = GetWorld()->GetNavigationSystem();
	}
	return NavigationSystem;
}

void ALevelEditorPawn::GetSphereAtCursor()
{
	if (bTopDownCamera && FindGround())
	{
		CalculateProjectedLocation();
		EditingIndex = Ground->GetPointAtLocation(ProjectedLocation);
	}
}

void ALevelEditorPawn::HandleMovementInput()
{
	const FVector AmountToMove = ConsumeMovementInputVector();
	AddActorWorldOffset(AmountToMove, true);
}

void ALevelEditorPawn::MoveHeld()
{
	if (bHoldingMovePoint && EditingIndex >= 0 && FindGround())
	{
		CalculateProjectedLocation();
		Ground->MovePoint(EditingIndex, ProjectedLocation);
	}
	else
	{
		EditingIndex = -1;
	}
}

void ALevelEditorPawn::SwitchCamera()
{
	FirstPersonCamera->SetActive(bTopDownCamera);
	TopDownCamera->SetActive(!bTopDownCamera);
	bTopDownCamera = !bTopDownCamera;
	PreviewItem->SetVisibility(!bTopDownCamera); // Only in FP mode

	Capsule->SetSimulatePhysics(false); // !bTopDownCamera (was producing weird behaviour though, Elmo leaning?)

	FTransform NewPlayerTransform;
	NewPlayerTransform.SetLocation(FVector(0.f, 0.f, 500.f));
	NewPlayerTransform.SetRotation(FRotator(0.f,0.f,0.f).Quaternion());
	SetActorTransform(NewPlayerTransform);

	FTransform NewCameraTransform;
	NewCameraTransform.SetLocation(FVector(0.f, 0.f, 50000.f));
	NewCameraTransform.SetRotation(FRotator(-90.f, 180.f, 180.f).Quaternion());
	NewCameraTransform.SetScale3D(5.f * FVector(1.f, 1.f, 1.f));
	TopDownCamera->SetWorldTransform(NewCameraTransform);

	if (FindController())
	{
		PlayerController->RotationInput = FRotator(0.f,0.f,0.f);
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(bTopDownCamera);;
		PlayerController->ServerRestartPlayer();
	}

}

void ALevelEditorPawn::AddButtonPressed()
{
	if (!FindGround()) return;
	if (bTopDownCamera)
	{
		CalculateProjectedLocation();
		Ground->AddPoint(ProjectedLocation);
	}
	else if(bCanPlaceItem)
	{
		Ground->AddMesh(InventoryComponent->GetSelectedItem().Name, PreviewItem->GetComponentTransform());
	}
}

void ALevelEditorPawn::MoveButtonPressed()
{
	if (bTopDownCamera && FindGround())
	{
		bHoldingMovePoint = true;
		GetSphereAtCursor();
	}
}

void ALevelEditorPawn::MoveButtonReleased()
{
	bHoldingMovePoint = false;
}

void ALevelEditorPawn::RemoveButtonPressed()
{
	if (bTopDownCamera && FindGround())
	{
		GetSphereAtCursor();
		Ground->RemovePoint(EditingIndex);
		EditingIndex = -1;
	}
	else if(!bTopDownCamera && FindGround() && SelectedMeshIndex >= 0)
	{
		Ground->RemoveMesh(SelectedMeshIndex);
		SelectedMeshIndex = -1;
	}
}

void ALevelEditorPawn::ToggleLoop()
{
	if (bTopDownCamera && FindGround())
	{
		Ground->ToggleClosedLoop();
	}
}

void ALevelEditorPawn::IncreaseTerrainHeight()
{
	if(bTopDownCamera)
	{
		ChangeTerrainHeight(1.f);
	}
	else
	{
		PlacingRotation.Yaw += 5.f;
	}
	
}

void ALevelEditorPawn::DecreaseTerrainHeight()
{
	if(bTopDownCamera)
	{
		ChangeTerrainHeight(-1.f);
	}
	else
	{
		PlacingRotation.Yaw -= 5.f;
	}
}

void ALevelEditorPawn::RotateTangentInput(float Amount)
{
	if(Amount == 0 || !FindGround()) return;
	if (bTopDownCamera)
	{
		GetSphereAtCursor();
		Ground->RotateTangent(EditingIndex, Amount);
	}
	else
	{
		const auto& NewSelectedItem = InventoryComponent->ChangeItem(Amount > 0);
		if(NewSelectedItem.Mesh)
		{
			PreviewItem->SetStaticMesh(NewSelectedItem.Mesh);
		}
	}
}

void ALevelEditorPawn::SnapToMesh()
{
	bSnapToMesh = true;
}

void ALevelEditorPawn::DeSnapToMesh()
{
	bSnapToMesh = false;
}

void ALevelEditorPawn::ChangeTerrainHeight(float Amount)
{
	GetSphereAtCursor();
	if(EditingIndex >= 0 && FindGround())
	{
		Ground->IncreaseHeight(EditingIndex, Amount * HeightStep);
	}
}

void ALevelEditorPawn::MoveX(float Amount)
{
	if(bTopDownCamera)
	{
		TopDownCamera->AddWorldOffset(50.f * Amount * FVector::YAxisVector);
		return;
	}
	const FVector Direction = GetActorRightVector();
	AddMovementInput(Direction, 20.f * Amount);
}

void ALevelEditorPawn::MoveY(float Amount)
{
	if(bTopDownCamera)
	{
		TopDownCamera->AddWorldOffset(50.f * Amount * FVector::XAxisVector);
		return;
	}
	const FVector Direction = GetActorForwardVector();
	AddMovementInput(Direction, 20.f * Amount);
}

void ALevelEditorPawn::MoveZ(float Amount)
{
	if (!bTopDownCamera)
	{
		AddMovementInput(FVector::ZAxisVector, 20.f * Amount);
	}
}

void ALevelEditorPawn::LookUp(float Amount)
{
	if (!bTopDownCamera)
	{
		AddControllerPitchInput(Amount);
	}
}

void ALevelEditorPawn::LookRight(float Amount)
{
	if (!bTopDownCamera)
	{
		AddControllerYawInput(Amount);
	}
}
