#pragma once
#include "CoreMinimal.h"
#include "AllanKart/Inventory/InventoryList.h"
#include "AllanKart/Inventory/InventoryStructs.h"
#include "AllanKart/Types/CheckpointData.h"
#include "AllanKart/Types/PlacedMesh.h"
#include "AllanKart/Types/TrackPoints.h"
#include "GameFramework/Actor.h"
#include "Ground.generated.h"

class ACheckpointActor;

UCLASS()
class ALLANKART_API AGround : public AActor
{
	GENERATED_BODY()
	
public:	
	AGround();
	virtual void Tick(float DeltaTime) override;

	/****
	 * Spline Points Editing
	 */
	UFUNCTION(BlueprintCallable)
	void AddPoint(FVector Location);

	// Get Point Index At Location
	UFUNCTION(BlueprintCallable)
	int32 GetPointAtLocation(const FVector& CursorLocation);

	UFUNCTION(BlueprintCallable)
	void MovePoint(const int32 PointIndex, const FVector& ToLocation);
	
	UFUNCTION(BlueprintCallable)
	void RemovePoint(const int32 PointIndex);
	
	UFUNCTION(BlueprintCallable)
	void RotateTangent(int32 PointIndex, float RotationAxis);

	UFUNCTION(BlueprintCallable)
	void IncreaseHeight(int32 PointIndex, float Amount);

	UFUNCTION(BlueprintCallable)
	void ToggleClosedLoop();

	UPROPERTY(EditAnywhere, Category="Debug")
	float StartOffset{0.f};
	
	UPROPERTY(EditAnywhere, Category="Debug")
	float EndOffset{0.f};

	UPROPERTY(EditAnywhere, Category="Debug")
	FVector ProceduralTrackOffset{0.f, 0.f, 0.f};
	
	/**
	 * Track Data
	 */
	UFUNCTION(BlueprintCallable)
	void SetNumberOfLaps(int32 Laps);

	// Save
	UFUNCTION(BlueprintCallable)
	FTrackPoints GetSplinePoints();

	// Load
	UFUNCTION(BlueprintCallable)
	bool SetSplinePoints(const FTrackPoints& TrackPoints);

	UFUNCTION(NetMulticast, Reliable)
	void SetTrackPoints(const FTrackPoints& TrackPoints);
	
	// Set checkpoints speed
	UFUNCTION(BlueprintCallable)
	void SetCheckpointsData(const TArray<FCheckpointData> TrackCheckpointsData);

	/***
	 * Internal functions, might be private in the future
	 */
	UFUNCTION(BlueprintCallable)
	void SetStartTransform(const FTransform& NewTransform);

	UPROPERTY(EditAnywhere, Category = "Level Editor")
	bool bEditing{ true };

	/*****
	 * Meshes
	 */

	UFUNCTION(BlueprintCallable)
	void AddMesh(FName Name, FTransform Transform);
	
	bool AddMesh(UStaticMesh* SelectedItem, FTransform Transform);

	/* Get Point Index At Location
	UFUNCTION(BlueprintCallable)
	int32 GetMeshAtLocation(const FVector& CursorLocation);

	UFUNCTION(BlueprintCallable)
	void MoveMesh(const int32 MeshIndex, const FVector& ToLocation);
	*/
	
	UFUNCTION(BlueprintCallable)
	void RemoveMesh(const int32 MeshIndex);
	
	
	UPROPERTY(VisibleInstanceOnly)
	TMap<FName, class UStaticMesh*> PlaceableItems;

protected:
	virtual void BeginPlay() override;

	void MovePlayerStart();

	

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryList> InventoryListClass;

	UPROPERTY(VisibleAnywhere)
	UInventoryList* InventoryList;

	UPROPERTY(EditAnywhere, Category = "Level Editor")
	float ScaleFactor{ 1.f };

	UPROPERTY(EditAnywhere, Category = "Level Editor")
	float DistanceMesh{500.f};

	UPROPERTY()
	FTransform StartTransform;

	float SplinesZOffset{ 0.f };

	FVector RoadSize{ 0.f,0.f,0.f };
	FVector RoadCenter{ 0.f,0.f,0.f };

	// Maximum distance between cursor and handle
	UPROPERTY(EditAnywhere, Category = "Level Editor")
	float ChooseHandleMaxDistance{ 1000.f };

	UPROPERTY(VisibleAnywhere)
	int32 NumberOfLaps{ 1 };

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* GroundBox;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* GroundMesh;

	UPROPERTY(VisibleAnywhere)
	class USplineComponent* RoadSplines;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* MeshesSceneComponent;

	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralTrack;

	UPROPERTY(EditAnywhere, Category = "Level Editor")
	class UStaticMesh* RoadMesh;

	// The mesh used to represent the position of the point
	UPROPERTY(EditAnywhere, Category = "Level Editor")
	
	UStaticMesh* SinglePointMesh;

	UPROPERTY(EditAnywhere, Category = "Level Editor")

	class UMaterialInstance* HandleMaterial;

	UPROPERTY(EditAnywhere, Category = "Level Editor")

	UMaterialInstance* SelectedHandleMaterial;

	UPROPERTY()
	int32 SelectedKey{ -1 };

	UPROPERTY(VisibleAnywhere)
	TArray<class USplineMeshComponent*> SplineMeshes;

	UPROPERTY(VisibleAnywhere)
	TArray<FCheckpointData> CheckpointsData;

	UPROPERTY()
	TArray<UStaticMeshComponent*> PointMeshes;

	UPROPERTY(EditAnywhere, Category = "AI Racing")
	TSubclassOf<class AWaypointActor> WaypointClass;

	UPROPERTY(EditAnywhere, Category = "AI Racing")
	float WaypointSeparation{500.f};

	UPROPERTY(EditAnywhere, Category = "AI Racing")
	float WaypointDefaultMaxSpeed{10000.f};

	// Defines "Braking zone" default size. Can be improved with % of speed to decrease
	UPROPERTY(EditAnywhere, Category = "AI Racing")
	float WaypointAdaptionZone{1000.f};
	
	UPROPERTY()
	TArray<ACheckpointActor*> CheckpointActors;
		
	void UpdateSplineMeshes();
	void UpdatePointHandlers();
	void IncludeCheckpoints();
	void IncludeWaypoints();

	// Static Meshes

	UPROPERTY()
	TArray<FPlacedMesh> PlacedMeshes;

	UPROPERTY()
	TMap<int32, class UPlacedMeshComponent*> PlacedMeshComponents;

	// Starts
	UPROPERTY(EditAnywhere)
	float GridSlotSize{300.f};

public:

	FORCEINLINE TArray<FInventoryItem> GetPlaceableItems() const { return InventoryList->ItemsList; }
	
	FTransform GetCheckpointTransform(int32 CheckpointNumber) const;
	
	FTransform GetGridTransformAtPosition(int32 Position);

	float GetDistanceToSpline(const FVector& Location) const;
	
};
