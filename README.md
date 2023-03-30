# Stereolabs ZED - Live Link Sample
ZED LiveLink Sample for Unreal Engine 5

# zed-livelink-mono

This sample can be used to send either camera tracking data or skeleton data from one camera into Unreal Engine 5 using Live link.
All the information is available [here](zed-livelink-mono/README.md)


# zed-livelink-fusion

This sample can be used to send skeleton data from multiples camera using the Fusion API (introduced in the v4.0 of the ZED SDK) into Unreal Engine 5 using Live link.
All the information is available [here](zed-livelink-fusion/README.md)


# Unreal Project

This unreal project shows you how to use the body tracking data sent through live link to animate a 3D model.

You can find more information on the Stereolabs [documentation](https://www.stereolabs.com/docs/livelink/)


## Connect to a livelink Source

In the "LivelinkMap" level, it no longer automatically connects to a live link at Start. Now, you need to do it manually by either selecting a Source in the Live link window or Pressing 'S' in the scene which will open a basic UI with all the sources available. You only need to click on the source to connect to. With this method, connecting to a source will automatically disconnect you from all other sources, meaning you can connect to only one source at a time.


## Select the remap asset

The ZED SDK now has multiple skeleton formats (Body 34, 38, and 70) available for animating a 3D model.
You need to make sure the correct remap asset is selected in the Anim Blueprint of Actor you are using in your level. Indeed, each body format has its own remap asset (as the name and numbers of joint is different).
The remap asset **must** be set in the **Anim blueprint**, in the **ZED Livelink** pose component.

For example, if you are using the Body format *Body_38*, open the **ABP_ZED_Manny** anim blueprint, select the **ZED LivelinkPose** component and, in the Detail panel, set the **Remap Asset** field to **RemapAssetBody38**

![](UnrealProject/images/remap_asset_selection.PNG)