// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProxyMeshAndLOD : ModuleRules
{
	public ProxyMeshAndLOD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ProxyMeshAndLOD",
			"ProxyMeshAndLOD/Variant_Platforming",
			"ProxyMeshAndLOD/Variant_Platforming/Animation",
			"ProxyMeshAndLOD/Variant_Combat",
			"ProxyMeshAndLOD/Variant_Combat/AI",
			"ProxyMeshAndLOD/Variant_Combat/Animation",
			"ProxyMeshAndLOD/Variant_Combat/Gameplay",
			"ProxyMeshAndLOD/Variant_Combat/Interfaces",
			"ProxyMeshAndLOD/Variant_Combat/UI",
			"ProxyMeshAndLOD/Variant_SideScrolling",
			"ProxyMeshAndLOD/Variant_SideScrolling/AI",
			"ProxyMeshAndLOD/Variant_SideScrolling/Gameplay",
			"ProxyMeshAndLOD/Variant_SideScrolling/Interfaces",
			"ProxyMeshAndLOD/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
