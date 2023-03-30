# Stereolabs ZED - Live Link Sample
ZED LiveLink Sample for Unreal

## Fusion API

This sample is using the Fusion API of the ZED SDK. This allows to fuse data between multiples cameras in order to increase the stability and the quality of the skeleton data.
Therefore, this sample requires the ZED SDK v4.0 to run.

## ZED Live Link tool

This tool is using the Live Link plugin to connect Unreal engine 5 to the ZED SDK, which enables you to send skeleton tracking data to UE5. This way, you can control a Skeletal Mesh in real-time using the Skeleton tracking module of the ZED SDK.

It can be send through local network on Windows and Linux computers.

## Getting started

 - First, download the latest version of the ZED SDK on [stereolabs.com](https://www.stereolabs.com/developers/)
- For more information, read the ZED [Documentation](https://www.stereolabs.com/docs) and [API documentation](https://www.stereolabs.com/docs/api/)


To compile the tool from source, you will require a source build of Unreal Engine.

## Download the Engine

To gain access to the UE engine code, please follow the steps below:

1. Create an account at GitHub: [Sign up](https://github.com/join)
2. Create an account at Epic Games: [Sign up](https://accounts.epicgames.com/login)
3. Login to your Epic Games account and associate your GitHub account in the settings by entering your username into the GitHub account field.
5. Login to your Github account and accept the Epic team invitation* (Can take a few seconds to appear).
4. Download or clone the [UE5 engine](https://github.com/EpicGames/UnrealEngine/tree/release) code.

> The link above will result in a 404 error if your GitHub account is not linked to an Epic Game one. Make sure to follow all the instructions above.

> *If the invitation isn't sent, clicking on the following link seems to trigger it: [Unreal Group](https://github.com/orgs/EpicGames).


## Build for Windows

### Compile from Source

1. Inside the root directory, run **Setup.bat**. This will check all the project dependencies and update them as needed.
2. Clone this repository inside of <Engine Install Folder>\Engine\Source\Programs.
3. Run **GenerateProjectFiles.bat** to create project files for the engine.
4. Load the project into Visual Studio by double-clicking on the UE5.sln file, then right click on the ZEDLiveLink target and select **Build**.
5. The binaries will be placed in <Engine Install Folder>\Engine\Binaries\Win64\ZEDLiveLink\.

## Build for Linux

### Compile from Source

1. Go into the folder you just downloaded/cloned and run **Setup.sh** from the terminal. This will check all the project dependencies and update them as needed.
2. Clone this repository inside of <Engine Install Folder>/Engine/Source/Programs.
3. In the Engine root directory, run **GenerateProjectFiles.sh**.
4. Run **make** to build the Engine.
5. Go to UnrealEngine/Engine/Build/BatchFiles and open a terminal.
6. Build the plugin with the command :
 ```bash
 $ ./RunUAT.sh BuildGraph -Script=Engine/Source/Programs/zed-livelink-plugin/BuildZEDLinux.xml -Target="Stage ZEDLiveLink Linux"
 ```
7. The binaries will be placed in <Engine Install Folder>/Engine/Binaries/Linux/ZEDLiveLink/.


### Using the Live Link app

The ZED Live link sample now requires a Config file (Json format) to run. This file contains all the parameters from the ZED SDK that can be modified in this sample.
By default, the sample will try to open a json called "ZEDFusionLiveLinkConfig.json" located next to this executable.

The Fusion API requires a calibration file to be able to fuse the data from multiple camera.
This file cam be generated using the ZED 360 tool available with the ZED SDK. Once your calibration file is ready, 
in the ZEDFusionLivelink.json file, set the value of **json_config_filename** to the path to this calibration file.

Your configuration file will look like this : 

```bash
{
    "InitParameters":
    {
      "resolution": "HD1080",
      "fps": 30,
      "depth_mode": "ULTRA"
    },
    "BodyTrackingParameters":
    {
      "detection_model": "HUMAN_BODY_MEDIUM",
      "body_format": "BODY_38",
      "body_selection": "FULL",
      "confidence": 15,
      "max_range": -1,
      "minimum_keypoints_threshold" : -1
    },
     "InitFusionParameters":
     {
       "json_config_filename": "",
       "output_performance_metrics": true,
       "verbose": false

     },
     "BodyTrackingFusionParameters":
     {
       "skeleton_minimm_allowed_keypoints": -1,
       "skeleton_minimum_allowed_camera": -1,
       "skeleton_smoothing": 0
     }
}
```

To change the name or location of the config file, add it as argument.
For example, to use a file called "ConfigFile.json", run :

```bash
$ ./ZEDLiveLinkFusion ConfigFile.json
```
3. You can see the connection status in the terminal.

- On Windows :

![](./doc_images/capture_zed_connected.jpg)


### Setting up a Unreal Engine Project



### Troubleshooting

- Your firewall might block the data stream. If you do not see the ZED Source in the Live Link window, try to disable it. 

- If the ZED Source is not detected in UnrealEngine, enable **Enable by default** in **Edit** -> **Project Settings** -> **UDP Messaging** -> **Enable by default**

![](./doc_images/EnableByDefault.jpg)

- You might also need to specify the "Unicast Endpoint" parameter if you are using the same machine for the ZED Livelink sample and the Unreal Project. In this case,
set the parameter to "127.0.0.1".


#### On Linux

If the plugin crashes at the start, try to run the ldd command onto the sl_zed_c.so library :

```bash
$ ldd libsl_zed_c.so
```
It will show all the dependencies required by the .so and allow you to install anything that might be missing (for example lib-usb).


Note that the c wrapper used for the Live link plugin is also available here : https://github.com/stereolabs/zed-c-api.

If you encounter issues running the live link plugin, do not hesitate to build the wrapper yourself and place it in the lib/win64 or /linux folder.
