// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;

public class ZEDLiveLink : ModuleRules
{
    public ZEDLiveLink(ReadOnlyTargetRules Target) : base(Target)
    {
        string CudaSDKPath = "";
        string ZEDSDKPath = "";

		bEnableUndefinedIdentifierWarnings = false;

		if (Target.Platform == UnrealTargetPlatform.Win64) {
            CudaSDKPath = System.Environment.GetEnvironmentVariable("CUDA_PATH", EnvironmentVariableTarget.Machine);
			ZEDSDKPath = System.Environment.GetEnvironmentVariable("ZED_SDK_ROOT_DIR", EnvironmentVariableTarget.Machine);
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            CudaSDKPath = "/usr/local/cuda";
            ZEDSDKPath = "/usr/local/zed";
		}

        string WrapperPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../lib/"));//ZEDSDKPath + "//bin//";

        PrivateIncludePaths.AddRange(new string[] { "Runtime/Launch/Public", "Runtime/Launch/Private" });

        // Unreal dependency modules
        PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"ApplicationCore",
			"Projects",
			"UdpMessaging",
			"LiveLinkInterface",
			"LiveLinkMessageBusFramework"
		});

		LoadZEDSDK(Target, ZEDSDKPath);
        LoadCUDA(Target, CudaSDKPath);
        LoadWrapper(Target, WrapperPath);
		LoadOpenCV(Target, WrapperPath);
	}

    public void LoadZEDSDK(ReadOnlyTargetRules Target, string DirPath)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            if (!Directory.Exists(DirPath))
            {
                string Err = string.Format("ZED SDK missing");
                System.Console.WriteLine(Err);
                throw new BuildException(Err);
            }

            // Set the paths to the SDK
            string[] LibrariesNames = Directory.GetFiles(Path.Combine(DirPath, "lib"));

            PrivateIncludePaths.Add(Path.Combine(DirPath, "include"));
			PublicAdditionalLibraries.Add(Path.Combine(DirPath, "lib", "sl_zed64.lib"));

        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            if (!Directory.Exists(DirPath))
            {
                string Err = string.Format("ZED SDK missing");
                System.Console.WriteLine(Err);
                throw new BuildException(Err);
            }

            // Set the paths to the SDK
            string[] LibrariesNames = Directory.GetFiles(Path.Combine(DirPath, "lib"), "*.so");

            PrivateIncludePaths.Add(Path.Combine(DirPath, "include"));

            foreach (string Library in LibrariesNames)
            {
                PublicAdditionalLibraries.Add(Library);
            }
        }
    }

    public void LoadCUDA(ReadOnlyTargetRules Target, string DirPath)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            if (!Directory.Exists(DirPath))
            {
                string Err = string.Format("CUDA SDK missing");
                System.Console.WriteLine(Err);
                throw new BuildException(Err);
            }

            string[] LibrariesName =  {
                                        "cuda",
                                        "cudart"
                                      };

            PublicIncludePaths.Add(Path.Combine(DirPath, "include"));

            foreach (string Library in LibrariesName)
            {
                PublicAdditionalLibraries.Add(Path.Combine(DirPath, "lib\\x64", Library + ".lib"));
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            if (!Directory.Exists(DirPath))
            {
                string Err = string.Format("CUDA SDK missing");
                System.Console.WriteLine(Err);
                throw new BuildException(Err);
            }

            string[] Libraries = Directory.GetFiles(Path.Combine(DirPath, "lib64"), "*.so");

            PublicIncludePaths.Add(Path.Combine(DirPath, "include"));

            foreach (string Library in Libraries)
            {
                if (Library != Path.Combine(DirPath, "lib64/libnvrtc.so") && Library != Path.Combine(DirPath, "lib64/libOpenCL.so")) PublicAdditionalLibraries.Add(Library);
            }
        }
    }

    public void LoadWrapper(ReadOnlyTargetRules Target, string DirPath)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            if (!Directory.Exists(DirPath))
            {
                string Err = string.Format("Wrapper missing");
                System.Console.WriteLine(Err);
                throw new BuildException(Err);
            }

			RuntimeDependencies.Add("$(TargetOutputDir)/sl_zed_c.dll", Path.Combine(DirPath + "/win64/sl_zed_c.dll"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            if (!Directory.Exists(DirPath))
            {
                string Err = string.Format("Wrapper missing");
                System.Console.WriteLine(Err);
                throw new BuildException(Err);
            }
		
		    PublicAdditionalLibraries.Add(DirPath + "/linux/libsl_zed_c.so");
            PublicAdditionalLibraries.Add(DirPath + "/linux/libzed_aruco.so");

            RuntimeDependencies.Add("$(TargetOutputDir)/ZEDLiveLink/libsl_zed_c.so", Path.Combine(DirPath + "/linux/libsl_zed_c.so"));
            RuntimeDependencies.Add("$(TargetOutputDir)/ZEDLiveLink/libzed_aruco.so", Path.Combine(DirPath + "/linux/libzed_aruco.so"));
        }
    }

	public void LoadOpenCV(ReadOnlyTargetRules Target, string DirPath)
	{
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			if (!Directory.Exists(DirPath))
			{
				string Err = string.Format("Wrapper missing");
				System.Console.WriteLine(Err);
				throw new BuildException(Err);
			}

			RuntimeDependencies.Add("$(TargetOutputDir)/zed_aruco.dll", Path.Combine(DirPath + "/win64/zed_aruco.dll"));
		}
	}
}
