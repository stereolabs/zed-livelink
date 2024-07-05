// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ZEDLiveLink : ModuleRules
{
	public ZEDLiveLink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


        PrivateIncludePaths.Add("ZEDLiveLink/Private");

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "LiveLinkInterface",
                "Messaging",
                "Engine",
                "Json",
                "JsonUtilities",
                "LiveLinkAnimationCore"
            }
			);


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Networking",
                "Sockets",
                "LiveLink",
                "AnimGraphRuntime",
                "LiveLinkInterface",
            }
            );

        if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
	}
}
