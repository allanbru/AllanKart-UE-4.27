// ReSharper disable All
#include "Ground.h"
#include "AllanKart/AI/WaypointActor.h"
#include "CheckpointActor.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SplineMeshActor.h"
#include "FasterSplineMeshBPLibrary.h"
#include "PlacedMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"
#include "ProceduralMeshComponent.h"
#include "Net/UnrealNetwork.h"


AGround::AGround()
{
	PrimaryActorTick.bCanEverTick = true;

	GroundBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Ground Collision Box"));
	GroundBox->SetBoxExtent(FVector(10000.f, 10000.f, 100.f));
	SetRootComponent(GroundBox);

	GroundBox->SetMobility(EComponentMobility::Movable);

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground Mesh"));
	GroundMesh->SetupAttachment(GroundBox);
	GroundMesh->SetRelativeScale3D(FVector(200.f, 200.f, 2.f));
	GroundMesh->SetMobility(EComponentMobility::Movable);
	GroundMesh->SetCanEverAffectNavigation(true);
	GroundMesh->bNavigationRelevant = true;
	GroundMesh->bFillCollisionUnderneathForNavmesh = true;


	RoadSplines = CreateDefaultSubobject<USplineComponent>(TEXT("Road Splines"));
	RoadSplines->SetupAttachment(GroundBox);
	RoadSplines->SetMobility(EComponentMobility::Movable);

	ProceduralTrack = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Track"));
	ProceduralTrack->bUseComplexAsSimpleCollision = true;
	ProceduralTrack->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProceduralTrack->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	ProceduralTrack->SetSimulatePhysics(false);
	ProceduralTrack->bNavigationRelevant = true;
	ProceduralTrack->bFillCollisionUnderneathForNavmesh = true;
	ProceduralTrack->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	ProceduralTrack->SetupAttachment(GetRootComponent());
	
	MeshesSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Meshes Scene Component"));
	MeshesSceneComponent->SetCanEverAffectNavigation(true);
	MeshesSceneComponent->bNavigationRelevant = true;
	GroundMesh->bFillCollisionUnderneathForNavmesh = true;
	MeshesSceneComponent->SetupAttachment(GetRootComponent());

	StartTransform.SetLocation(FVector(0.f,0.,200.f));
	StartTransform.SetRotation(FQuat::Identity);
	StartTransform.SetScale3D(FVector(1.f,1.f,1.f));
	
}

void AGround::BeginPlay()
{
	Super::BeginPlay();

	if(InventoryListClass)
	{
		InventoryList = NewObject<UInventoryList>(this, InventoryListClass);
	}
	if(InventoryList)
	{
		PlaceableItems.Empty();
		for(const auto& Item : InventoryList->ItemsList)
		{
			if(Item.Mesh)
			{
				PlaceableItems.Add(Item.Name, Item.Mesh);
			}
		}
	}
	
	if (RoadMesh)
	{
		RoadCenter = RoadMesh->GetBoundingBox().GetCenter();
		RoadSize = RoadMesh->GetBoundingBox().GetExtent();
		SplinesZOffset = RoadSize.Z / 2;
	}

	SplinesZOffset = 0;
	RoadSplines->ClearSplinePoints();
	UpdateSplineMeshes();
	
	RoadSplines->AddLocalOffset(FVector(0.f, 0.f, -1+SplinesZOffset));

	//Set PlayerStart
	MovePlayerStart();

	UpdateSplineMeshes();
	if(!bEditing)
	{
		ProceduralTrack->AddWorldOffset(ProceduralTrackOffset);
	}

	GroundBox->SetCanEverAffectNavigation(false);
	GroundBox->bDynamicObstacle = false;
	GroundBox->bFillCollisionUnderneathForNavmesh = false;
	GroundBox->UpdateNavigationBounds();
	
	ProceduralTrack->UpdateNavigationBounds();
	ProceduralTrack->bFillCollisionUnderneathForNavmesh = false;
	ProceduralTrack->SetCanEverAffectNavigation(false);
	ProceduralTrack->SetCanEverAffectNavigation(true);
}

void AGround::MovePlayerStart()
{
	if (RoadSplines && RoadSplines->GetNumberOfSplinePoints() >= 2)
	{
		FVector Start = RoadSplines->GetWorldLocationAtSplinePoint(0);
		FVector Turn1 = RoadSplines->GetWorldLocationAtSplinePoint(1);
		FRotator Direction = (Turn1 - Start).ToOrientationRotator();
		FTransform NewTransform;
		NewTransform.SetLocation(Start + Direction.Vector() * 100.f + FVector::ZAxisVector * 150.f);
		NewTransform.SetRotation(Direction.Quaternion());
		StartTransform = NewTransform;
	}
	AActor* PlayerStartActor = UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass());
	if (PlayerStartActor)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(PlayerStartActor);
		if (PlayerStart)
		{
			PlayerStart->GetCapsuleComponent()->SetMobility(EComponentMobility::Movable);
			PlayerStart->SetActorTransform(StartTransform);
			PlayerStart->AddActorWorldOffset(ProceduralTrackOffset);
		}
	}
}

void AGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGround::AddPoint(FVector Location)
{
	RoadSplines->AddSplinePoint(Location, ESplineCoordinateSpace::World, true);
	UpdateSplineMeshes();
}

int32 AGround::GetPointAtLocation(const FVector& CursorLocation)
{
	if (RoadSplines && RoadSplines->GetNumberOfSplinePoints() > 0 && (RoadSplines->FindLocationClosestToWorldLocation(CursorLocation, ESplineCoordinateSpace::World) - CursorLocation).Size() <= ChooseHandleMaxDistance)
	{
		int32 ClosestKey = FMath::Floor<int32>(RoadSplines->FindInputKeyClosestToWorldLocation(CursorLocation));
		const int32 NextKey = (ClosestKey + 1) % RoadSplines->GetNumberOfSplinePoints();
		const float DistClosest = FVector::DistSquared(CursorLocation, RoadSplines->GetLocationAtSplinePoint(ClosestKey, ESplineCoordinateSpace::World));
		const float DistNext = FVector::DistSquared(CursorLocation, RoadSplines->GetLocationAtSplinePoint(NextKey, ESplineCoordinateSpace::World));
		if(DistNext < DistClosest)
		{
			ClosestKey = NextKey;
		}
		
		if (ClosestKey != SelectedKey && ClosestKey != -1 && ClosestKey < PointMeshes.Num() && PointMeshes[ClosestKey] != nullptr)
		{
			if (HandleMaterial && SelectedKey != -1 && SelectedKey < PointMeshes.Num() && PointMeshes[SelectedKey] != nullptr && PointMeshes[SelectedKey]->GetMaterial(0))
			{
				PointMeshes[SelectedKey]->SetMaterial(0, HandleMaterial);
			}
			if(SelectedHandleMaterial && PointMeshes[ClosestKey]->GetMaterial(0)) PointMeshes[ClosestKey]->SetMaterial(0, SelectedHandleMaterial);
		}
		SelectedKey = ClosestKey;
		return ClosestKey;
	}
	
	if (HandleMaterial && SelectedKey != -1 && SelectedKey < PointMeshes.Num() && PointMeshes[SelectedKey] != nullptr && PointMeshes[SelectedKey]->GetMaterial(0))
	{
		PointMeshes[SelectedKey]->SetMaterial(0, HandleMaterial);
	}
	SelectedKey = -1;
	return -1;
}

void AGround::MovePoint(const int32 PointIndex, const FVector& ToLocation)
{
	if (RoadSplines && PointIndex < RoadSplines->GetNumberOfSplinePoints())
	{
		RoadSplines->SetLocationAtSplinePoint(PointIndex, ToLocation, ESplineCoordinateSpace::World, true);
		UpdateSplineMeshes();
	}
}

void AGround::RemovePoint(const int32 PointIndex)
{
	if (PointIndex >= 0 && RoadSplines && PointIndex < RoadSplines->GetNumberOfSplinePoints())
	{
		RoadSplines->RemoveSplinePoint(PointIndex, true);
		if (PointMeshes.Num() > 0 && PointIndex < PointMeshes.Num() && PointMeshes[PointIndex] != nullptr)
		{
			PointMeshes[PointIndex]->DestroyComponent();
			PointMeshes.RemoveAt(PointIndex);
		}
		
		int32 SegmentIndex = PointIndex - 1;
		if (SegmentIndex >= 0 && SplineMeshes.Num() > 0 && SegmentIndex < SplineMeshes.Num() && SplineMeshes[SegmentIndex] != nullptr)
		{
			SplineMeshes[SegmentIndex]->DestroyComponent();
			SplineMeshes.RemoveAt(SegmentIndex);
		}

		UpdateSplineMeshes();
	}
}

void AGround::RotateTangent(int32 PointIndex, float RotationAxis)
{
	if (RoadSplines && PointIndex < RoadSplines->GetNumberOfSplinePoints())
	{
		FVector CurrentTangent = RoadSplines->GetTangentAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
		if(CurrentTangent.IsNearlyZero())
		{
			CurrentTangent = FVector::YAxisVector;
		}
		FVector RotatedTangent = CurrentTangent.RotateAngleAxis(5.f * RotationAxis, FVector::ZAxisVector);
		RoadSplines->SetTangentAtSplinePoint(PointIndex, RotatedTangent, ESplineCoordinateSpace::World, true);
		UpdateSplineMeshes();
	}
}

void AGround::IncreaseHeight(int32 PointIndex, float Amount)
{
	if(RoadSplines && PointIndex >= 0 && PointIndex < RoadSplines->GetNumberOfSplinePoints())
	{
		const FVector CurrentPosition = RoadSplines->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
		const FVector AddedPosition = Amount * FVector::ZAxisVector;
		RoadSplines->SetLocationAtSplinePoint(PointIndex, CurrentPosition + AddedPosition, ESplineCoordinateSpace::World);
		UpdateSplineMeshes();
	}
}

void AGround::ToggleClosedLoop()
{
	if (RoadSplines)
	{
		bool bNewClosedLoop = !RoadSplines->IsClosedLoop();
		RoadSplines->SetClosedLoop(bNewClosedLoop, true);
		if (SplineMeshes.Num() > 0 && !bNewClosedLoop)
		{
			int32 RemovingIndex = SplineMeshes.Num() - 1;
			SplineMeshes[RemovingIndex]->DestroyComponent();
			SplineMeshes.RemoveAt(RemovingIndex);
		}
		UpdateSplineMeshes();
	}
}

void AGround::SetNumberOfLaps(int32 Laps)
{
	if (Laps > 0)
	{
		NumberOfLaps = Laps;
	}
}

FTrackPoints AGround::GetSplinePoints()
{
	if (!RoadSplines) return FTrackPoints();
	FTrackPoints TrackPoints;
	
	for (int32 i = 0; i < RoadSplines->GetNumberOfSplinePoints(); i++)
	{
		FVector Location, Tangent;
		RoadSplines->GetLocationAndTangentAtSplinePoint(i, Location, Tangent, ESplineCoordinateSpace::World);
		TrackPoints.PointLocations.Add(Location);
		TrackPoints.PointTangents.Add(Tangent);
	}
	
	TrackPoints.PlacedMeshes = PlacedMeshes;
	TrackPoints.bClosedLoop = RoadSplines->IsClosedLoop();
	TrackPoints.StartTransform = StartTransform;
	TrackPoints.NumberOfLaps = NumberOfLaps;
	if(CheckpointsData.Num() == TrackPoints.PointLocations.Num())
	{
		TrackPoints.CheckpointsData = CheckpointsData;
	}
	
	return TrackPoints;
}

bool AGround::SetSplinePoints(const FTrackPoints& TrackPoints)
{
	if (RoadSplines && TrackPoints.PointLocations.Num() > 0 && TrackPoints.PointLocations.Num() == TrackPoints.PointTangents.Num())
	{
		RoadSplines->ClearSplinePoints();
		for (auto& SplineMeshPoint : SplineMeshes)
		{
			SplineMeshPoint->DestroyComponent();
		}
		SplineMeshes.Empty();
		for (auto& PointMesh : PointMeshes)
		{
			PointMesh->DestroyComponent();
		}
		
		PointMeshes.Empty();
		for (int32 i = 0; i < TrackPoints.PointLocations.Num(); i++)
		{
			RoadSplines->AddSplinePoint(TrackPoints.PointLocations[i], ESplineCoordinateSpace::World, true);
		}
		
		for (int32 i = 0; i < TrackPoints.PointTangents.Num(); i++)
		{
			RoadSplines->SetTangentAtSplinePoint(i, TrackPoints.PointTangents[i], ESplineCoordinateSpace::World, true);
		}

		PlacedMeshes.Empty();
		PlacedMeshComponents.Empty();
		if(MeshesSceneComponent)
		{
			TArray<USceneComponent*> ChildrenComponents;
			MeshesSceneComponent->GetChildrenComponents(true, ChildrenComponents);
			for(auto& ChildComponent: ChildrenComponents)
			{
				ChildComponent->DestroyComponent();
			}
		}
		
		for(auto PlacedMesh : TrackPoints.PlacedMeshes)
		{
			if(PlaceableItems.Contains(PlacedMesh.StaticMeshName) && PlaceableItems[PlacedMesh.StaticMeshName] != nullptr)
			{
				AddMesh(PlacedMesh.StaticMeshName, PlacedMesh.WorldTransform);
			}
		}
		
		RoadSplines->SetClosedLoop(TrackPoints.bClosedLoop);
		StartTransform = TrackPoints.StartTransform;
		NumberOfLaps = TrackPoints.NumberOfLaps;
		CheckpointsData = TrackPoints.CheckpointsData;

		UpdateSplineMeshes();

		if (!bEditing && HasAuthority())
		{
			IncludeCheckpoints();
			IncludeWaypoints();
			ProceduralTrack->UpdateBounds();
		}
		else if(!bEditing)
		{
			ProceduralTrack->UpdateBounds();
			ProceduralTrack->UpdateNavigationBounds();
		}

		MovePlayerStart();
		return true;
	}
	return false;
}

void AGround::SetTrackPoints_Implementation(const FTrackPoints& TrackPoints)
{
	SetSplinePoints(TrackPoints);
}

void AGround::SetCheckpointsData(const TArray<FCheckpointData> TrackCheckpointsData)
{
	if(TrackCheckpointsData.Num() == RoadSplines->GetNumberOfSplinePoints())
	{
		CheckpointsData = TrackCheckpointsData;
	}
}

void AGround::SetStartTransform(const FTransform& NewTransform)
{
	StartTransform = NewTransform;
	MovePlayerStart();
}

void AGround::AddMesh(FName Name, FTransform Transform)
{
	if(PlaceableItems.Contains(Name) && PlaceableItems[Name] != nullptr)
	{
		if(AddMesh(PlaceableItems[Name], Transform))
		{
			PlacedMeshes.Last().StaticMeshName = Name;
		}
	}
}

bool AGround::AddMesh(UStaticMesh* SelectedItem, FTransform Transform)
{
	UPlacedMeshComponent* NewComponent = NewObject<UPlacedMeshComponent>(this);
	if (SelectedItem && MeshesSceneComponent && NewComponent)
	{
		NewComponent->SetMobility(EComponentMobility::Movable);
		NewComponent->SetStaticMesh(SelectedItem);
		NewComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
		NewComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		NewComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		NewComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECR_Block);
		NewComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECR_Block);
		NewComponent->SetEnableGravity(false);
		NewComponent->SetSimulatePhysics(false);
		NewComponent->bFillCollisionUnderneathForNavmesh = false;
		NewComponent->AttachToComponent(MeshesSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
		NewComponent->RegisterComponentWithWorld(GetWorld());
		NewComponent->SetVisibility(true);

		const int32 NewComponentIndex = PlacedMeshes.Num();
		NewComponent->PlacedMeshIndex = NewComponentIndex;
		NewComponent->ComponentTags.Add(FName("StackableItem"));
		NewComponent->ComponentTags.Add(FName("EditableItem"));
		NewComponent->SetCanEverAffectNavigation(true);

		NewComponent->SetWorldTransform(Transform, false, nullptr, ETeleportType::TeleportPhysics);

		FPlacedMesh PlacedMesh;
		PlacedMesh.PlacedMeshId = NewComponentIndex;
		PlacedMesh.StaticMeshName = SelectedItem->GetFName();
		PlacedMesh.WorldTransform = Transform;
		
		PlacedMeshes.Add(PlacedMesh);
		PlacedMeshComponents.Emplace(NewComponentIndex, NewComponent);
		return true;
	}
	return false;
}

void AGround::RemoveMesh(const int32 MeshIndex)
{
	if(MeshIndex >= 0 && PlacedMeshComponents.Contains(MeshIndex) && PlacedMeshComponents[MeshIndex])
	{
		PlacedMeshComponents[MeshIndex]->DestroyComponent();
		bool bFound = false;
		const int PreviousNum = PlacedMeshes.Num();
		int32 IndexToSearch = FMath::Min(MeshIndex, PlacedMeshes.Num() - 1);
		while(IndexToSearch >= 0 && !bFound)
		{
			if(PlacedMeshes[IndexToSearch].PlacedMeshId == MeshIndex)
			{
				PlacedMeshes.RemoveAt(IndexToSearch);
				bFound = true;
			}
			else
			{
				IndexToSearch--;
			}
		}
	}
}

void AGround::UpdateSplineMeshes()
{
	if (!RoadSplines) return;

		RoadSplines->UpdateSpline();
		ProceduralTrack->ClearAllMeshSections();
	
		if(RoadSplines->GetNumberOfSplinePoints() < 2)
		{
			ProceduralTrack->ClearAllMeshSections();
			return;
		}

	
		TArray<FSplineMeshParams2> MeshParams;
	
		UFasterSplineMeshBPLibrary::CreateProceduralSplineMeshesParams(
			RoadSplines,
			MeshParams,
			DistanceMesh,
			FVector2D(0.f, 0.f),
			FVector2D(0.f,0.f),
			StartOffset,
			EndOffset,
			false,
			FVector2D(1.f, 1.f),
			ESplineMeshAxis::X
		);

		UFasterSplineMeshBPLibrary::CreateProceduralMeshFromSplineMeshesParams(
			ProceduralTrack,
			RoadMesh,
			MeshParams,
			true,
			false,
			0		
		);
		
	UpdatePointHandlers();
}

void AGround::UpdatePointHandlers()
{
	if (!RoadSplines || SinglePointMesh == nullptr) return;
	for (int32 i = 0; i < RoadSplines->GetNumberOfSplinePoints(); i++)
	{
		if (i > PointMeshes.Num() - 1)
		{	
			UStaticMeshComponent* PointHandler = NewObject<UStaticMeshComponent>();
			if (PointHandler)
			{
				PointHandler->SetMobility(EComponentMobility::Movable);
				PointHandler->SetStaticMesh(SinglePointMesh);
				PointHandler->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				PointHandler->AttachToComponent(RoadSplines, FAttachmentTransformRules::KeepRelativeTransform);
				PointHandler->RegisterComponentWithWorld(GetWorld());
				PointHandler->SetWorldLocation(RoadSplines->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World) + FVector(0.f,0.f,SplinesZOffset * 2));
				PointHandler->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
				PointHandler->SetVisibility(bEditing);

				if (PointHandler->GetMaterial(0) && HandleMaterial)
				{
					PointHandler->SetMaterial(0, HandleMaterial);
				}

				PointMeshes.AddUnique(PointHandler);
			}
		}
		

		if(i < PointMeshes.Num() && PointMeshes[i] != nullptr)
		{
			const FVector PointLocation = RoadSplines->GetWorldLocationAtSplinePoint(i);
			PointMeshes[i]->SetWorldLocation(PointLocation);
		}
	}

}

void AGround::IncludeCheckpoints()
{
	if (!RoadSplines || SinglePointMesh == nullptr) return;
	UWorld* World = GetWorld();
	if (!World) return;
	float CheckpointYExtent = 16.f;
	if (RoadMesh)
	{
		CheckpointYExtent = RoadMesh->GetBoundingBox().GetExtent().Y;
	}
	for (int32 i = 0; i < RoadSplines->GetNumberOfSplinePoints(); i++)
	{
		FTransform SpawnTransform = RoadSplines->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World);
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ECheckpointType CheckpointType = ECheckpointType::ECT_Sector;
		if (i == 0)
		{
			CheckpointType = (RoadSplines->IsClosedLoop()) ? ECheckpointType::ECT_StartAndFinish : ECheckpointType::ECT_Start;
		}
		else if (i == RoadSplines->GetNumberOfSplinePoints() - 1 && !RoadSplines->IsClosedLoop())
		{
			CheckpointType = ECheckpointType::ECT_Finish;
		}

		ACheckpointActor* NewCheckPoint = World->SpawnActor<ACheckpointActor>(ACheckpointActor::StaticClass(), RoadSplines->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::World), Parameters);
		if(NewCheckPoint)
		{
			NewCheckPoint->AddActorWorldOffset(ProceduralTrackOffset);
			NewCheckPoint->TriggerBox->SetBoxExtent(FVector(200.f, CheckpointYExtent * 2.f, 200.f));
			NewCheckPoint->CheckpointType = CheckpointType;
			NewCheckPoint->SetCheckpointNumber(i);
			NewCheckPoint->SetNumberOfCheckpoints(RoadSplines->GetNumberOfSplinePoints());
			NewCheckPoint->SetNumberOfLaps(NumberOfLaps);

			CheckpointActors.Add(NewCheckPoint);
		}
	}
}

void AGround::IncludeWaypoints()
{
	if(!WaypointClass) return;
	const float TotalDistance = RoadSplines->GetSplineLength();
	
	// Default using checkpoint speeds
	if(CheckpointsData.Num() == RoadSplines->GetNumberOfSplinePoints() && CheckpointsData.Num() >= 3)
	{
		int32 WaypointNumber = 0;
		int32 CurrentCheckpoint = 0;
		float CurrentPosition = 0.f;
		float PreviousCheckpointPosition = RoadSplines->GetDistanceAlongSplineAtSplinePoint(CurrentCheckpoint);
		float NextCheckpointPosition = RoadSplines->GetDistanceAlongSplineAtSplinePoint(CurrentCheckpoint + 1);

		while(CurrentPosition < TotalDistance && CurrentCheckpoint < RoadSplines->GetNumberOfSplinePoints() - 1)
		{
			if(CurrentPosition >= NextCheckpointPosition)
			{
				CurrentCheckpoint += 1;
				PreviousCheckpointPosition = RoadSplines->GetDistanceAlongSplineAtSplinePoint(CurrentCheckpoint);
                NextCheckpointPosition = RoadSplines->GetDistanceAlongSplineAtSplinePoint(CurrentCheckpoint + 1);
			}

			float TargetSpeed = WaypointDefaultMaxSpeed;
			const float DistanceToNextCheckpoint = (NextCheckpointPosition - CurrentPosition);
			const float InvWaypointAdaptionZone = 1 / WaypointAdaptionZone;
			
			if(DistanceToNextCheckpoint < WaypointAdaptionZone)
			{
				const float Alpha = DistanceToNextCheckpoint * InvWaypointAdaptionZone;
				TargetSpeed = (1 - Alpha) * CheckpointsData[CurrentCheckpoint + 1].TargetSpeed + Alpha * WaypointDefaultMaxSpeed;
			}

			FTransform WaypointTransform = RoadSplines->GetTransformAtDistanceAlongSpline(CurrentPosition, ESplineCoordinateSpace::World);
			AWaypointActor* NewWaypoint = GetWorld()->SpawnActor<AWaypointActor>(WaypointClass, WaypointTransform);
			if(NewWaypoint)
			{
				NewWaypoint->AddActorWorldOffset(ProceduralTrackOffset);
				NewWaypoint->SetWaypointSpeed(TargetSpeed);
				NewWaypoint->WaypointSequenceNumber = WaypointNumber;
				WaypointNumber++;
			}
			
			CurrentPosition += WaypointSeparation;
		}
	}
	// Alternate, for unvalidated tracks
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TRACK NOT VALIDATED"));
		float Distance = 0.f;
		while(Distance < TotalDistance)
		{
			FTransform WaypointTransform = RoadSplines->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			AActor* NewWaypoint = GetWorld()->SpawnActor<AActor>(WaypointClass, WaypointTransform);
			if(NewWaypoint) NewWaypoint->AddActorWorldOffset(ProceduralTrackOffset);
			Distance += WaypointSeparation;
		}		
	}
	
	
}

FTransform AGround::GetCheckpointTransform(int32 CheckpointNumber) const
{
	if(CheckpointNumber < CheckpointActors.Num() && CheckpointActors[CheckpointNumber] != nullptr)
	{
		return CheckpointActors[CheckpointNumber]->GetActorTransform();
	}
	return StartTransform;
}

FTransform AGround::GetGridTransformAtPosition(int32 Position)
{
	float TrackLength = RoadSplines->GetSplineLength();
	if(TrackLength > 0)
	{
		FTransform GridSlot = RoadSplines->GetTransformAtDistanceAlongSpline(TrackLength - (Position + 1) * GridSlotSize, ESplineCoordinateSpace::World, false);
		GridSlot.SetLocation(GridSlot.GetLocation() + FVector::ZAxisVector * (50.f));
		return GridSlot;
	}
	return GetCheckpointTransform(0);
}

float AGround::GetDistanceToSpline(const FVector& Location) const
{
	const FVector ClosestPoint = RoadSplines->FindLocationClosestToWorldLocation(Location, ESplineCoordinateSpace::World);
	return (Location - ClosestPoint).Size();
}

