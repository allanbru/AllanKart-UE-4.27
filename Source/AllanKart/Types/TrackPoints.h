#pragma once
#include "CoreMinimal.h"
#include "CheckpointData.h"
#include "PlacedMesh.h"
#include "TrackPoints.generated.h"

// Stores all spline's locations and tangents of a track. Later we should do FTrackSplinePoint, aggregating Loc, Tang and CP data
USTRUCT(BlueprintType)
struct FTrackPoints 
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVector> PointLocations;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVector> PointTangents;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FPlacedMesh> PlacedMeshes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FCheckpointData> CheckpointsData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bClosedLoop;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform StartTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 NumberOfLaps;

};