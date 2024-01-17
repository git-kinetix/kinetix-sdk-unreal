// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class KinetixRuntime : ModuleRules
{
	public KinetixRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"glTFRuntime",
				"PoseSearch"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EnhancedInput",
				"Projects",
				"HTTP",
				"Json",
				"JsonUtilities",
				"JsonBlueprintUtilities",
				"DeveloperSettings",
				"RenderCore"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		// PublicAdditionalLibraries.Add(Path.Combine(PluginDirectory, "Binaries", "ThirdParty", "Kinetix", "Kinanim.lib"));
		// PublicAdditionalLibraries.Add(Path.Combine(PluginDirectory, "Binaries", "ThirdParty", "Kinetix", "KinanimLibrary.lib"));
		// PublicDelayLoadDLLs.Add("Kinanim.dll");
		// RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/Kinetix/Kinanim.dll");
	}
}
