// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PortalOcclusionDemo : ModuleRules
{
	public PortalOcclusionDemo(ReadOnlyTargetRules Target) : base(Target)
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
			"PortalOcclusionDemo",
			"PortalOcclusionDemo/Variant_Horror",
			"PortalOcclusionDemo/Variant_Horror/UI",
			"PortalOcclusionDemo/Variant_Shooter",
			"PortalOcclusionDemo/Variant_Shooter/AI",
			"PortalOcclusionDemo/Variant_Shooter/UI",
			"PortalOcclusionDemo/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
