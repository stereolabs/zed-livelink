#include "FrameData.h"
#include "ZEDSkeletonRetarget.h"

FrameData::FrameData(FString frameData)
{
    // Create a variable to store the parsed JSON data
    TSharedPtr<FJsonObject> JsonObject;

    // Create a reader for the JSON data
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(frameData);

    bIsValid = false;
    Timestamp = 0;
    // Try to deserialize the JSON data
    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        // Successfully parsed the JSON data

        int role = -1;
        if (!JsonObject->TryGetNumberField("role", role))
        {
            bIsValid = false;
            return;
        }

        if (!JsonObject->TryGetNumberField("timestamp", Timestamp))
        {
            bIsValid = false;
            return;
        }

        if(!JsonObject->TryGetNumberField("frame_id", FrameID))
        {
            bIsValid = false;
            return;
        }

        int value = 0;
        if (!JsonObject->TryGetNumberField("coordinate_system", value))
        {
            bIsValid = false;
            return;
        }
        CoordinateSystem = (EZEDCoordinateSystem)value;

        if (!JsonObject->TryGetNumberField("coordinate_unit", value))
        {
            bIsValid = false;
            return;
        }
        CoordinateUnit = (EZEDCoordinateUnit)value;

        if (role == (int)EZEDLiveLinkRole::Camera)
        {
            int64 SerialNumber = -1;
            if (!JsonObject->TryGetNumberField("serial_number", SerialNumber))
            {
                bIsValid = false;
                return;
            }

            Subject = "SN" + FString::FromInt(SerialNumber);
            SubjectRole = ULiveLinkCameraRole::StaticClass();

            const TSharedPtr<FJsonObject>* CameraPositionArray;
            if (!JsonObject->TryGetObjectField("camera_position", CameraPositionArray))
            {
                bIsValid = false;
                return;
            }
            FVector CameraPosition = ConvertCoordinateUnitToUE(CoordinateUnit, FVector(CameraPositionArray->Get()->GetNumberField("x"), CameraPositionArray->Get()->GetNumberField("y"), CameraPositionArray->Get()->GetNumberField("z")));

            const TSharedPtr<FJsonObject>* CameraOrientationArray;
            if (!JsonObject->TryGetObjectField("camera_orientation", CameraOrientationArray))
            {
                bIsValid = false;
                return;
            }
            FQuat CameraOrientation = FQuat(CameraOrientationArray->Get()->GetNumberField("x"), CameraOrientationArray->Get()->GetNumberField("y"), CameraOrientationArray->Get()->GetNumberField("z"), CameraOrientationArray->Get()->GetNumberField("w"));

            CameraTransform.SetLocation(CameraPosition);
            CameraTransform.SetRotation(CameraOrientation);

            CameraTransform = ConvertCoordinateSystemToUE(CoordinateSystem, CameraTransform);

            bIsValid = true;
        }
        else if (role == (int)EZEDLiveLinkRole::Animation)
        {
            int DetectionID = -1;
            
            if (!JsonObject->TryGetNumberField("id", DetectionID))
            {
                bIsValid = false;
                return;
            }

            Subject = FString::FromInt(DetectionID);
            SubjectRole = ULiveLinkAnimationRole::StaticClass();

            EZEDBodyFormat ZEDBodyFormat = EZEDBodyFormat::Body_38;
            
            if (!JsonObject->TryGetNumberField("body_format", value))
            {
                bIsValid = false;
                return;
            }
            ZEDBodyFormat = (EZEDBodyFormat)value;

            if (!JsonObject->TryGetNumberField("tracking_state", value))
            {
                bIsValid = false;
                return;
            }
            BodyTrackingState = (EZEDTrackingState)value;
            

            if (ZEDBodyFormat == EZEDBodyFormat::Body_34)// BODY_34
            {
                NbKeypoints = 34;
                Keypoints = Keypoints34;
                ParentsIdx = Parents34Idx;
                TargetBones = TargetBone34;
            }
            else
            {
                NbKeypoints = 38;
                Keypoints = Keypoints38;
                ParentsIdx = Parents38Idx;
                TargetBones = TargetBone38;
            }

            // Root transform of the skeleton
            auto rootPositionJsonValue = JsonObject->GetObjectField("global_root_posititon");
            FVector rootPosition = ConvertCoordinateUnitToUE(CoordinateUnit, FVector(rootPositionJsonValue->GetNumberField("x"), rootPositionJsonValue->GetNumberField("y"), rootPositionJsonValue->GetNumberField("z")));

            auto rootOrientationJsonValue = JsonObject->GetObjectField("global_root_orientation");
            FQuat rootOrientation = FQuat(rootOrientationJsonValue->GetNumberField("x"), rootOrientationJsonValue->GetNumberField("y"), rootOrientationJsonValue->GetNumberField("z"), rootOrientationJsonValue->GetNumberField("w"));

            if (rootPosition.ContainsNaN())
            {
                rootPosition = FVector::ZeroVector;
            }
            if (rootOrientation.ContainsNaN())
            {
                rootOrientation = FQuat::Identity;
            }

            TArray< TSharedPtr<FJsonValue>> LocalPositions = JsonObject->GetArrayField("local_position_per_joint");
            TArray< TSharedPtr<FJsonValue>> LocalOrientations = JsonObject->GetArrayField("local_orientation_per_joint");
            
            FTransform RootTransform;
            rootOrientation.Normalize();
            RootTransform.SetRotation(rootOrientation);
            RootTransform.SetLocation(rootPosition);

            RootTransform = ConvertCoordinateSystemToUE(CoordinateSystem, RootTransform);
            BoneTransform.Push(RootTransform);

            // Local position and rotation of each keypoint
            for (int i = 1; i < NbKeypoints; i++)
            {
                FQuat Orientation = FQuat(LocalOrientations[i]->AsObject()->GetNumberField("x"), LocalOrientations[i]->AsObject()->GetNumberField("y"), LocalOrientations[i]->AsObject()->GetNumberField("z"), LocalOrientations[i]->AsObject()->GetNumberField("w"));
                FVector Position = ConvertCoordinateUnitToUE(CoordinateUnit, FVector(LocalPositions[i]->AsObject()->GetNumberField("x"), LocalPositions[i]->AsObject()->GetNumberField("y"), LocalPositions[i]->AsObject()->GetNumberField("z")));

                if (Position.ContainsNaN())
                {
                    Position = FVector::ZeroVector;
                }
                if (Orientation.ContainsNaN())
                {
                    Orientation = FQuat::Identity;
                }

                FTransform Transform;
                Orientation.Normalize();
                Transform.SetRotation(Orientation);
                Transform.SetLocation(Position);

                //UE_LOG(LogTemp, Warning, TEXT("%s"), *Transform.ToString());
                Transform = ConvertCoordinateSystemToUE(CoordinateSystem, Transform);

                BoneTransform.Push(Transform);
            }

            TArray< TSharedPtr<FJsonValue>> KeypointConfidence = JsonObject->GetArrayField("keypoint_confidence");

            for (int i = 0; i < NbKeypoints; i++)
            {
                float Confidence = KeypointConfidence[i]->AsNumber();

                FVector VecConf(Confidence, Confidence, Confidence);
                FTransform TfConf;
                TfConf.SetLocation(VecConf);
                BoneTransform.Push(TfConf);
            }
            bIsValid = true;
        }
        else
        {
            bIsValid = false;
        }
    }
    else
    {
        // Failed to parse JSON data
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON data"));
        bIsValid = false;
    }
}

void FrameData::GetCoordinateTransform(EZEDCoordinateSystem coord_system, FMatrix& coordinateMatrix) {
    coordinateMatrix.SetIdentity();

    // set desired coordinate system
    switch (coord_system) {
    case EZEDCoordinateSystem::Image:
        coordinateMatrix.SetIdentity();
        break;
    case EZEDCoordinateSystem::Left_Handed_Y_Up:
        coordinateMatrix.M[1][1] = -1;
        break;
    case EZEDCoordinateSystem::Right_Handed_Y_Up:
        coordinateMatrix.M[1][1] = -1;
        coordinateMatrix.M[2][2] = -1;
        break;
    case EZEDCoordinateSystem::Right_Handed_Z_Up:
        coordinateMatrix.M[1][1] = 0;
        coordinateMatrix.M[1][2] = -1;
        coordinateMatrix.M[2][1] = 1;
        coordinateMatrix.M[2][2] = 0;
        break;
    case EZEDCoordinateSystem::Right_Handed_Z_Up_X_Fwd:
        coordinateMatrix.M[0][0] = 0;
        coordinateMatrix.M[1][1] = 0;
        coordinateMatrix.M[2][2] = 0;
        coordinateMatrix.M[1][2] = -1;
        coordinateMatrix.M[0][1] = -1;
        coordinateMatrix.M[2][0] = 1;

        break;
    case EZEDCoordinateSystem::Left_Handed_Z_Up:
        coordinateMatrix.M[0][0] = 0;
        coordinateMatrix.M[0][1] = 1;
        coordinateMatrix.M[1][0] = 0;
        coordinateMatrix.M[1][1] = 0;
        coordinateMatrix.M[1][2] = -1;
        coordinateMatrix.M[2][0] = 1;
        coordinateMatrix.M[2][1] = 0;
        coordinateMatrix.M[2][2] = 0;
        break;
    default:
        break;
    }
}

//Convert Transform to UE coordinate frame (Left Z up)
FTransform FrameData::ConvertCoordinateSystemToUE(EZEDCoordinateSystem InFrame, FTransform InTransform)
{
    FMatrix tmp;
    tmp.SetIdentity();

    FMatrix coordTransf;
    coordTransf.SetIdentity();

    GetCoordinateTransform(InFrame, tmp); //src 
    GetCoordinateTransform(EZEDCoordinateSystem::Left_Handed_Z_Up, coordTransf); // dst is unreal coordinate system

    FMatrix mat;
    mat.SetIdentity();
    mat = (coordTransf.Inverse() * tmp);
    FMatrix mat_out = mat * InTransform.ToMatrixWithScale() * mat.Inverse();

    FTransform out;
    out.SetFromMatrix(mat_out);
    return out;
}


//Convert vector to UE coordinate unit (centimeter)
FVector FrameData::ConvertCoordinateUnitToUE(EZEDCoordinateUnit InUnit, FVector InVector)
{
    float factor = 1;

    switch (InUnit)
    {
        case EZEDCoordinateUnit::Meter:
            factor = 100;
            break;
        case EZEDCoordinateUnit::Centimeter:
            factor = 1;
            break;
        case EZEDCoordinateUnit::Millimeter:
            factor = 0.1f;
            break;
        case EZEDCoordinateUnit::Inch:
            factor = 2.54f;
            break;
        case EZEDCoordinateUnit::Foot:
            factor = 30.48f;
            break;
        default:
            factor = 0.1f;
    }
    return InVector * factor;
}
