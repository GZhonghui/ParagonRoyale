// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ParagonRoyal : ModuleRules
{
	public ParagonRoyal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PublicDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore", "Sockets", "Networking", "MediaAssets" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.Add("$(ProjectDir)/Include");

		PublicAdditionalLibraries.Add("$(ProjectDir)/Lib/libprotobuf.lib");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
