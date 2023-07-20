#pragma once

#include "CoreMinimal.h"
#include "PlacedMesh.generated.h"

USTRUCT(BlueprintType)
struct FPlacedMesh
{

	GENERATED_BODY()

    UPROPERTY(VisibleAnywhere)
    int32 PlacedMeshId;

	UPROPERTY(VisibleAnywhere)
	FName StaticMeshName;

	UPROPERTY(EditAnywhere)
	FTransform WorldTransform;

};
