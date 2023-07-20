#pragma once
#include "CoreMinimal.h"
#include "InventoryStructs.generated.h"

UENUM(BlueprintType)
enum class EInventoryCategory : uint8
{
	EIC_NONE UMETA(DisplayName = "None"),
	EIC_Landscape UMETA(DisplayName = "Landscape"),
	EIC_Trees UMETA(DisplayName = "Trees"),
	EIC_Grass UMETA(DisplayName = "Grass"),
	EIC_Props UMETA(DisplayName = "Props"),
	EIC_Roads UMETA(DisplayName = "Roads"),
	EIC_Buildings UMETA(DisplayName = "Buildings"),
	EIC_MAX UMETA(DisplayName = "DefaultMax")
	
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInventoryCategory Category{EInventoryCategory::EIC_NONE};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;
	
};

USTRUCT(BlueprintType)
struct FPlacedItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Index;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform Transform;
};