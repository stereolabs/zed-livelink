// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class ZEDUnrealLiveLinkTarget : TargetRules
{
	public ZEDUnrealLiveLinkTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		BuildEnvironment = TargetBuildEnvironment.Unique;

		GlobalDefinitions.Add("ALLOW_UDP_MESSAGING_SHIPPING=1");

		ExtraModuleNames.AddRange( new string[] { "ZEDUnrealLiveLink" } );
		
		if (bBuildEditor)
		{
			ExtraModuleNames.AddRange(new string[] { "ZEDUnrealLiveLinkEditor" });
		}
	}
}
