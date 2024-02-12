# Stereolabs ZED - Live Link Sample
ZED LiveLink Sample for Unreal Engine 5

# zed-livelink-mono

This sample can be used to send either camera tracking data or skeleton data from one camera into Unreal Engine 5 using Live link.

# zed-livelink-fusion

This sample can be used to send skeleton data from multiples camera using the Fusion API (introduced in the v4.0 of the ZED SDK) into Unreal Engine 5 using Live link.

# Unreal Plugin

This plugin allows you how to use the body tracking data sent through live link to animate a 3D model.

- Copy the **ZEDLiveLink** folder in the **Plugins** Directory of your project.

The plugin should be visible (and enabled) in the Plugins window in your Unreal Project.

![](images/zed_plugin.jpg)

## Connect to a livelink Source

In the "LivelinkMap" level, it no longer automatically connects to a live link at Start. Now, you need to do it manually by selecting a Source in the Live link window.

- Open the Live Link window, click on **Source** -> **Stereolabs Source**. Set the IP address of the sender, and the type of connection (Unicast or multicast) and click on **Create Source**.

![](images/connect_source.jpg)