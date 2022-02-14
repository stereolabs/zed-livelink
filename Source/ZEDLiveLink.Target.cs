// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;
using System.IO;

public class ZEDLiveLinkTarget : TargetRules
{
	public ZEDLiveLinkTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Program;
    	LinkType = TargetLinkType.Monolithic;

    	bShouldCompileAsDLL = false;
		SolutionDirectory = "Programs/LiveLink";

    	// We only need minimal use of the engine for this plugin
    	bBuildDeveloperTools = false;
		bUseMallocProfiler = false;
		bBuildWithEditorOnlyData = true;
        bCompileAgainstEngine = false;
		bCompileAgainstCoreUObject = true;
		bCompileICU = false;
    	bIsBuildingConsoleApplication = true;


		if (Target.Platform == UnrealTargetPlatform.Win64)
        	ExeBinariesSubFolder = "ZEDLiveLink/";

		this.LaunchModuleName = "ZEDLiveLink";

    }
}
