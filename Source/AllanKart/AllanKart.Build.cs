// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AllanKart : ModuleRules
{
	public AllanKart(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "MultiplayerSessions", "PhysXVehicles", "AIModule", "NavigationSystem", "ProceduralMeshComponent", "FasterSplineMesh" });

		PrivateDependencyModuleNames.AddRange(new string[] { "FasterSplineMesh" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
	