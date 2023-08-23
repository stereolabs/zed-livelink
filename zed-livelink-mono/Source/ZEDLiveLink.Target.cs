// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;
using System;
using System.IO;
#if !UE_5_0_OR_LATER
using Tools.DotNETCommon;
#else
using EpicGames.Core;
#endif
using System.Runtime.CompilerServices;

public class ZEDLiveLinkTarget : TargetRules
{
	/// <summary>
	/// Finds the innermost parent directory with the provided name. Search is case insensitive.
	/// </summary>
	string InnermostParentDirectoryPathWithName(string ParentName, string CurrentPath)
	{
		DirectoryInfo ParentInfo = Directory.GetParent(CurrentPath);

		if (ParentInfo == null)
		{
			throw new DirectoryNotFoundException("Could not find parent folder '" + ParentName + "'");
		}

		// Case-insensitive check of the parent folder name.
		if (ParentInfo.Name.ToLower() == ParentName.ToLower())
		{
			return ParentInfo.ToString();
		}

		return InnermostParentDirectoryPathWithName(ParentName, ParentInfo.ToString());
	}

	/// <summary>
	/// Returns the path to this .cs file.
	/// </summary>
	string GetCallerFilePath([CallerFilePath] string CallerFilePath = "")
	{
		if (CallerFilePath.Length == 0)
		{
			throw new FileNotFoundException("Could not find the path of our .cs file");
		}

		return CallerFilePath;
	}

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

		this.LaunchModuleName = "ZEDLiveLink";

		// This .cs file must be inside the source folder of this Program. We later use this to find other key directories.
		string TargetFilePath = GetCallerFilePath();

		// We need to avoid failing to load DLL due to looking for EngineDir() in non-existent folders.
		// By having it build in the same directory as the engine, it will assume the engine is in the same directory
		// as the program, and because this folder always exists, it will not fail the check inside EngineDir().

		// Because this is a Program, we assume that this target file resides under a "Programs" folder.
		string ProgramsDir = InnermostParentDirectoryPathWithName("Programs", TargetFilePath);

		// We assume this Program resides under a Source folder.
		string SourceDir = InnermostParentDirectoryPathWithName("Source", ProgramsDir);

		// The program is assumed to reside inside the "Engine" folder.
		string EngineDir = InnermostParentDirectoryPathWithName("Engine", SourceDir);

		// The default Binaries path is assumed to be a sibling of "Source" folder.
		string DefaultBinDir = Path.GetFullPath(Path.Combine(SourceDir, "..", "Binaries", Platform.ToString()));

		// We assume that the engine exe resides in Engine/Binaries/[Platform]
		string EngineBinariesDir = Path.Combine(EngineDir, "Binaries", Platform.ToString());

		// Now we calculate the relative path between the default output directory and the engine binaries,
		// in order to force the output of this program to be in the same folder as th engine.
		ExeBinariesSubFolder = (new DirectoryReference(EngineBinariesDir)).MakeRelativeTo(new DirectoryReference(DefaultBinDir));

		string OutputName = LaunchModuleName;

		string PostBuildBinDir = Path.Combine(DefaultBinDir, "ZEDLiveLink");
		if (Target.Platform.ToString() == "Win64")
		{
			// Copy binaries
			PostBuildSteps.Add(string.Format("echo Copying {0} to {1}...", EngineBinariesDir, PostBuildBinDir));
			PostBuildSteps.Add(string.Format("xcopy /y /i /v \"{0}\\{1}.*\" \"{2}\" 1>nul", EngineBinariesDir, OutputName, PostBuildBinDir));
			PostBuildSteps.Add(string.Format("xcopy /y /i /v \"{0}\\{1}.*\" \"{2}\" 1>nul", EngineBinariesDir, "sl_zed_c", PostBuildBinDir));
			PostBuildSteps.Add(string.Format("xcopy /y /i /v \"{0}\\{1}.*\" \"{2}\" 1>nul", EngineBinariesDir, "zed_opencv", PostBuildBinDir));

			string ConfigFilePath = Path.Combine(Path.GetDirectoryName(RulesCompiler.GetFileNameFromType(this.GetType())), "..");
			PostBuildSteps.Add(string.Format("echo Copying {0} to {1}...", ConfigFilePath, PostBuildBinDir));
			PostBuildSteps.Add(string.Format("xcopy /y /i /v \"{0}\\{1}.*\" \"{2}\" 1>nul", ConfigFilePath, "ZEDLiveLinkConfig", PostBuildBinDir));
		}
	}
}