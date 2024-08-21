#pragma once

#include<vector>
#include "Json.h"
#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "FrameData.generated.h"

UENUM()
enum class EZEDBodyFormat : uint8
{
    Body_34 = 1,
    Body_38 = 38
};

UENUM()
enum class EZEDLiveLinkRole : uint8
{
    Transform = 0,
    Camera,
    Animation
};

UENUM()
enum class EZEDTrackingState : uint8 
{
    Off = 0, /**< The tracking is not yet initialized.\n The object id is not usable. */
    Ok, /**< The object is tracked. */
    Searching, /**< The object could not be detected in the image and is potentially occluded.\n The trajectory is estimated. */
    Terminate /**< This is the last searching state of the track.\n The track will be deleted in the next sl::Camera.retrieveObjects(). */
};


UENUM()
enum class EZEDCoordinateUnit {
    Millimeter, /**< International System (1/1000 meters) */
    Centimeter, /**< International System (1/100 meters) */
    Meter, /**< International System (1 meter)*/
    Inch, /**< Imperial Unit (1/12 feet) */
    Foot, /**< Imperial Unit (1 foot)*/
};

UENUM()
enum class EZEDCoordinateSystem {
    Image, /**< Standard coordinates system in computer vision.\n Used in OpenCV: see <a href="http://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html">here</a>. */
    Left_Handed_Y_Up, /**< Left-handed with Y up and Z forward.\n Used in Unity with DirectX. */
    Right_Handed_Y_Up, /**< Right-handed with Y pointing up and Z backward.\n Used in OpenGL. */
    Right_Handed_Z_Up, /**< Right-handed with Z pointing up and Y forward.\n Used in 3DSMax. */
    Left_Handed_Z_Up, /**< Left-handed with Z axis pointing up and X forward.\n Used in Unreal Engine. */
    Right_Handed_Z_Up_X_Fwd, /**< Right-handed with Z pointing up and X forward.\n Used in ROS (REP 103). */
};

class FrameData
{
public:

    uint64 Timestamp = 0;
    int FrameID;
    TSubclassOf<ULiveLinkRole> SubjectRole;
    FString Subject;

    EZEDCoordinateSystem CoordinateSystem;
    EZEDCoordinateUnit CoordinateUnit;

    // Camera data
    FTransform CameraTransform;

    // Animation data
    TArray<FTransform> BoneTransform;
    TArray<int>ParentsIdx;
    int NbKeypoints = -1;
    TMap<int, FName> Keypoints;
    TArray<FName> TargetBones;
    EZEDTrackingState BodyTrackingState = EZEDTrackingState::Terminate;
    FrameData(FString frameData);

    bool bIsValid = false;

private:
    void GetCoordinateTransform(EZEDCoordinateSystem coord_system, FMatrix& coordinateMatrix);
    FTransform ConvertCoordinateSystemToUE(EZEDCoordinateSystem InFrame, FTransform InTransform);
    FVector ConvertCoordinateUnitToUE(EZEDCoordinateUnit InUnit, FVector InVector);

    void Deserialize(TSharedRef<TJsonReader<>> Reader);
};