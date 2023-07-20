#pragma once
#include "CoreMinimal.h"
#include "CheckpointData.generated.h"

/*
 * Stores checkpoint data and further interpolates
 **/
USTRUCT(BlueprintType)
struct FCheckpointData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CheckpointNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float TargetSpeed;
};
