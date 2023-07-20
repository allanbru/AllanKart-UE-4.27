#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"

#include "AllanKartTypes.generated.h"

USTRUCT(BlueprintType)
struct FKartDriver
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	USkeletalMesh* DriverMesh;

	UPROPERTY(BlueprintReadWrite)
	FTransform DefaultTransform;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UAnimInstance> DriverAnimInstance;

	UPROPERTY(BlueprintReadWrite)
	UAnimMontage* DrivingMontage;

};

UENUM(BlueprintType)
enum class ESelectedDriver : uint8 {
	ESD_Elmo UMETA(DisplayName = "Elmo"),
	ESD_Garibaldo UMETA(DisplayName = "Garibaldo"),
	ESD_MAX UMETA(DisplayName = "DefaultMAX")
};