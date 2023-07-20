// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryList.h"
#include "Components/ActorComponent.h"
#include "InventoryStructs.h"
#include "InventoryComponent.generated.h"

ENUM_RANGE_BY_COUNT(EInventoryCategory, EInventoryCategory::EIC_MAX)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALLANKART_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FInventoryItem GetSelectedItem() const { return SelectedItem; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EInventoryCategory GetSelectedCategory() const { return SelectedCategory; }

	UFUNCTION(BlueprintCallable)
	void SetCategory(const EInventoryCategory Category);

	UFUNCTION(BlueprintCallable)
	FInventoryItem ChangeItem(const bool bForward);
	
protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryList> InventoryListClass;

	UPROPERTY(VisibleAnywhere)
	UInventoryList* InventoryList;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FInventoryItem> InventoryItems;
	
	UPROPERTY(VisibleAnywhere)
	FInventoryItem SelectedItem;

	UPROPERTY(VisibleAnywhere)
	EInventoryCategory SelectedCategory{EInventoryCategory::EIC_NONE};
	
	TMap<EInventoryCategory, TArray<FInventoryItem>> MapByCategory;

	int32 SelectedIndex{0};

	FInventoryItem SetSelectedItem(const int32 NewIndex);

	
public:

};
