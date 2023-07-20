// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	for(auto Category : TEnumRange<EInventoryCategory>())
	{
		if(!MapByCategory.Contains(Category))
		{
			TArray<FInventoryItem> NewArray;
			MapByCategory.Emplace(Category, NewArray);
		}
	}

	if(InventoryListClass)
	{
		InventoryList = NewObject<UInventoryList>(this, InventoryListClass);
	}
	
	if(InventoryList)
	{
		InventoryItems = InventoryList->ItemsList;
	}

	int32 CurrentIndex = 0;
	TArray<int32> IndexesToRemove;
	for(auto& Item : InventoryItems)
	{
		if(Item.Mesh && MapByCategory.Contains(Item.Category))
		{
			MapByCategory[Item.Category].Add(Item);
			CurrentIndex++;
		}
		else
		{
			IndexesToRemove.Add(CurrentIndex);
			CurrentIndex++;
		}
	}
	
	for(int32 Index = 0; Index < IndexesToRemove.Num(); Index++)
	{
		//As items get removed, the size of InventoryItems changes...
		InventoryItems.RemoveAt(IndexesToRemove[Index] - Index);
	}
	
	if(InventoryItems.Num() > 0 && InventoryItems[0].Mesh)
	{
		SetSelectedItem(0);
	}
}

FInventoryItem UInventoryComponent::SetSelectedItem(const int32 NewIndex)
{
	if(NewIndex < InventoryItems.Num())
	{
		SelectedIndex = NewIndex;
		SelectedItem = InventoryItems[SelectedIndex];
	}
	return SelectedItem;
}

void UInventoryComponent::SetCategory(const EInventoryCategory Category)
{
	if(MapByCategory.Contains(Category) && MapByCategory[Category].Num() > 0)
	{
		SelectedCategory = Category;
		InventoryItems = MapByCategory[SelectedCategory];
		SetSelectedItem(0);
	}
}

FInventoryItem UInventoryComponent::ChangeItem(const bool bForward)
{
	if(InventoryItems.Num() <= 1) return SelectedItem;
	int32 NextItem = (bForward) ? SelectedIndex + 1 : SelectedIndex - 1;
	NextItem = (NextItem + InventoryItems.Num()) % InventoryItems.Num();
	return SetSelectedItem(NextItem);
}

