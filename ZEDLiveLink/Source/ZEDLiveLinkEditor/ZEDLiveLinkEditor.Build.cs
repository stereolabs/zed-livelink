//======= Copyright (c) Stereolabs Corporation, All rights reserved. ===============

using UnrealBuildTool;

public class ZEDLiveLinkEditor : ModuleRules
{
	public ZEDLiveLinkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateIncludePaths.Add("ZEDLiveLinkEditor/Private");

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","ZEDLiveLink","AnimGraph",
				"AnimGraphRuntime",
				"BlueprintGraph"});


			 PrivateDependencyModuleNames.AddRange(
	 new string[]
	 {
		 "CoreUObject",
		 "Engine",
		 "Slate",
		 "SlateCore",
		"UnrealEd",
		"GraphEditor",
		"PropertyEditor",
		"EditorStyle",
		"ContentBrowser",
		"ZEDLiveLink",
		"AnimGraph",
		"AnimGraphRuntime",
		 // ... add private dependencies that you statically link with here ...
	 }
	 );
}
}
