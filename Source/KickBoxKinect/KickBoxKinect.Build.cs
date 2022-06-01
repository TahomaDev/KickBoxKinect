// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KickBoxKinect : ModuleRules
{
	public KickBoxKinect(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		
		PublicSystemLibraryPaths.Add("./Lib");
		//		PublicAdditionalLibraries.Add("'filepath'\DWrite.lib");
		PublicAdditionalLibraries.Add("Kinect20.lib");
		PublicAdditionalLibraries.Add("Kinect20.Face.lib");
	}
}
