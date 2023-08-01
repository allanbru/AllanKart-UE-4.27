// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveManager.h"

#include "AllanKart/LevelEditor/LevelSaveGame.h"
#include "Kismet/GameplayStatics.h"

bool USaveManager::SaveLevel(FTrackPoints TrackPoints, const FText& TrackName, const uint8 NumberOfLaps)
{
    if(TrackPoints.PointLocations.Num() <= 1) return false;

    USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(ULevelSaveGame::StaticClass());
    if(SaveGameObject)
    {
        ULevelSaveGame* LevelSaveGameObject = Cast<ULevelSaveGame>(SaveGameObject);
        if(LevelSaveGameObject)
        {
            TrackPoints.NumberOfLaps = NumberOfLaps;
            LevelSaveGameObject->TrackPoints = TrackPoints;
            return UGameplayStatics::SaveGameToSlot(LevelSaveGameObject, TrackName.ToString(), 0);

        }
    }

    return false;
}

bool USaveManager::LoadLevel(const FText& TrackName, FTrackPoints& TrackPoints)
{
    USaveGame* LoadedFile = UGameplayStatics::LoadGameFromSlot(TrackName.ToString(), 0);
    if(LoadedFile)
    {
        ULevelSaveGame* LoadedLevel = Cast<ULevelSaveGame>(LoadedFile);
        if(LoadedLevel)
        {
            TrackPoints = LoadedLevel->TrackPoints;
            return true;
        }
    }
    return false;
}

AGround* USaveManager::FindGround(const UObject* WorldReferenceObject, bool& FindResult)
{
    FindResult = false;
    if(!WorldReferenceObject) return nullptr;
    AActor* GroundCandidate = UGameplayStatics::GetActorOfClass(WorldReferenceObject, AGround::StaticClass());
    if(GroundCandidate)
    {
        AGround* GroundReference = Cast<AGround>(GroundCandidate);
        if(GroundReference != nullptr)
            FindResult = true;
            return GroundReference;
    }
    return nullptr;
}
