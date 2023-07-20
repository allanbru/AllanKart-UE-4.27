// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PlacedMeshComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALLANKART_API UPlacedMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:

	int32 PlacedMeshIndex{-1};
	
};
