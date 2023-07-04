#include "LiveLinkOrientationsRemapAsset.h"

#include "Roles/LiveLinkAnimationTypes.h"
#include "BonePose.h"
#include "Util.hpp"

void ULiveLinkOrientationsRemapAsset::SetHeightOffset(float Offset)
{
	ManualHeightOffset = Offset;
}

void ULiveLinkOrientationsRemapAsset::EnableBoneScaling(bool BoneScaling)
{
	bBoneScalingEnabled = BoneScaling;
}

void ULiveLinkOrientationsRemapAsset::EnableStickAvatarOnFloor(bool bEnableStickAvatarOnFloor)
{
	bStickAvatarOnFloor = bEnableStickAvatarOnFloor;
}

FCompactPoseBoneIndex ULiveLinkOrientationsRemapAsset::GetCPIndex(int32 idx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames) {
	FName BoneName = TransformedBoneNames[idx];
	const int32 MeshIndex = OutPose.GetBoneContainer().GetPoseBoneIndexForBoneName(BoneName);
	if (MeshIndex != INDEX_NONE)
	{
		FCompactPoseBoneIndex CPIndex = OutPose.GetBoneContainer().MakeCompactPoseIndex(
			FMeshPoseBoneIndex(MeshIndex));
		return CPIndex;
	}
	return (FCompactPoseBoneIndex)INDEX_NONE;
}

float ULiveLinkOrientationsRemapAsset::ComputeRootTranslationFactor(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, const FLiveLinkAnimationFrameData* InFrameData) 
{
	float avatarTotalTranslation = 0.f;
	float SDKTotalTranslation = 0.f;
	
	if (NbKeypoints <= 34)
	{
		for (int32 i = 22; i < 24; i++)
		{
			FTransform BoneTransform = InFrameData->Transforms[i];
			FCompactPoseBoneIndex CPIndex = GetCPIndex(i, OutPose, TransformedBoneNames);
			if (CPIndex != INDEX_NONE)
			{
				avatarTotalTranslation += OutPose[CPIndex].GetTranslation().Size();
				SDKTotalTranslation += BoneTransform.GetTranslation().Size();
			}
		}
	}
	else
	{
		for (int32 i = 19; i < 23; i++)
		{
			FTransform BoneTransform = InFrameData->Transforms[i];
			FCompactPoseBoneIndex CPIndex = GetCPIndex(i, OutPose, TransformedBoneNames);
			if (CPIndex != INDEX_NONE)
			{
				avatarTotalTranslation += OutPose[CPIndex].GetTranslation().Size();
				SDKTotalTranslation += BoneTransform.GetTranslation().Size();
			}
		}
	}

	float factor = avatarTotalTranslation / SDKTotalTranslation;
	float scale = 1.f;
	FCompactPoseBoneIndex CPIndexRoot = GetCPIndex(0, OutPose, TransformedBoneNames);
	if (CPIndexRoot != INDEX_NONE)
		scale = OutPose[CPIndexRoot].GetScale3D().Z;
	return FMath::Abs(scale * factor);
}

void ULiveLinkOrientationsRemapAsset::putInRefPose(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames){
    for (int32 i = 0; i < TransformedBoneNames.Num(); i++)
    {
        FCompactPoseBoneIndex CPIndex = GetCPIndex(i, OutPose, TransformedBoneNames);
        if (CPIndex != INDEX_NONE)
        {
            auto RefBoneTransform = OutPose.GetRefPose(CPIndex);
            OutPose[CPIndex].SetRotation(RefBoneTransform.GetRotation());
        }
    }
}

void ULiveLinkOrientationsRemapAsset::propagateRestPoseRotations(int32 parentIdx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, TArray<int32> SourceBoneParents, FQuat restPoseRot, bool inverse){
	for (int32 i = 0; i < SourceBoneParents.Num(); i++){
        FCompactPoseBoneIndex CPIndex = GetCPIndex(i, OutPose, TransformedBoneNames);
        if(SourceBoneParents[i] == parentIdx)
        {
            FQuat restPoseRotChild;
            if (CPIndex != INDEX_NONE)
            {   
                FQuat jointRotation = restPoseRot * OutPose[CPIndex].GetRotation();
                OutPose[CPIndex].SetRotation(jointRotation);
                if (!inverse)
                    restPoseRotChild = restPoseRot * OutPose.GetRefPose(CPIndex).GetRotation();
                else
                    restPoseRotChild = OutPose.GetRefPose(CPIndex).GetRotation().Inverse() * restPoseRot;
            }
            else
                restPoseRotChild = restPoseRot; // in case the parent bone doesn't appear in 
                                                // the Unreal avatar, we propagate its own restposerot to 
                                                // its children 
            propagateRestPoseRotations(i, OutPose, TransformedBoneNames, SourceBoneParents, restPoseRotChild, inverse);
        }
	}
}

// Take Live Link data and apply it to skeleton bones in UE.
void ULiveLinkOrientationsRemapAsset::BuildPoseFromZEDAnimationData(float DeltaTime,
    const FLiveLinkSkeletonStaticData* InSkeletonData,
    const FLiveLinkAnimationFrameData* InFrameData,
    FCompactPose& OutPose,
    USkeletalMeshComponent* SkeletalMesh)
{
    const TArray<FName>& SourceBoneNames = InSkeletonData->BoneNames;
    const TArray<int32>& SourceBoneParents = InSkeletonData->BoneParents;

    TArray<FName, TMemStackAllocator<>> TransformedBoneNames;
    TransformedBoneNames.Reserve(SourceBoneNames.Num());


	if (NbKeypoints < 0) // only the first time.
	{
		// Check the size of the input data to know which body format is used.
		if (InFrameData->Transforms.Num() == Keypoints38.Num() * 2) // body_38
		{
			NbKeypoints = 38;
			Keypoints = Keypoints38;
			ParentsIdx = parents38Idx;
		}
#if 0
		else if (InFrameData->Transforms.Num() == Keypoints70.Num() * 2)// BODY_70
		{
			NbKeypoints = 70;
			Keypoints = Keypoints70;
			ParentsIdx = parents70Idx;
		}
#endif
		else if (InFrameData->Transforms.Num() == Keypoints34.Num() * 2)// BODY_34
		{
			NbKeypoints = 34;
			Keypoints = Keypoints34;
			ParentsIdx = parents34Idx;
		}
		else
		{
			NbKeypoints = 38;
			Keypoints = Keypoints38;
			ParentsIdx = parents38Idx;
		}
	}

    // Find remapped bone names and cache them for fast subsequent retrieval.
    for (const FName& SrcBoneName : SourceBoneNames)
    {
		if (!SrcBoneName.ToString().ToLower().Contains("conf") && !Keypoints.FindKey(SrcBoneName))
		{
			UE_LOG(LogTemp, Fatal, TEXT("Bone names mismatch between remap asset and live link sender. %s"), *SrcBoneName.ToString());
		}

        FName* TargetBoneName = BoneNameMap.Find(SrcBoneName);
        if (TargetBoneName == nullptr)
        {
            FName NewName = GetRemappedBoneName(SrcBoneName);
            TransformedBoneNames.Add(NewName);
            BoneNameMap.Add(SrcBoneName, NewName);
        }
        else
        {
            TransformedBoneNames.Add(*TargetBoneName);
        }

    }

	// Apply an offset to put the feet of the ground and offset "floating" avatars.
	if (bStickAvatarOnFloor && InFrameData->Transforms[NbKeypoints / 2 + *Keypoints.FindKey(FName("LEFT_ANKLE"))].GetLocation().X > 0 && InFrameData->Transforms[NbKeypoints / 2 + *Keypoints.FindKey(FName("RIGHT_ANKLE"))].GetLocation().X > 0) { //if both foot are visible/detected
		if (SkeletalMesh) {

			FVector LeftFootPosition;
			FVector RightFootPosition;

			if (InFrameData->Transforms.Num() == 34 * 2) // body 34
			{
				LeftFootPosition = SkeletalMesh->GetBoneLocation(TransformedBoneNames[21]);
				RightFootPosition = SkeletalMesh->GetBoneLocation(TransformedBoneNames[25]);
			}
			else // body 38
			{
				LeftFootPosition = SkeletalMesh->GetBoneLocation(TransformedBoneNames[24]);
				RightFootPosition = SkeletalMesh->GetBoneLocation(TransformedBoneNames[25]);
			}

			FHitResult HitLeftFoot;
			bool RaycastLeftFoot = GetWorld()->LineTraceSingleByObjectType(OUT HitLeftFoot, LeftFootPosition + FVector(0, 0, 200), LeftFootPosition - FVector(0, 0, 200),
				FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic));

			FHitResult HitRightFoot;
			bool RaycastRightFoot = GetWorld()->LineTraceSingleByObjectType(OUT HitRightFoot, RightFootPosition + FVector(0, 0, 200), RightFootPosition - FVector(0, 0, 200),
				FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic));

			float LeftFootFloorDistance = 0;
			float RightFootFloorDistance = 0;

			// Compute the distance between one foot and the ground (the first static object found by the ray cast).
			if (RaycastLeftFoot)
			{
				LeftFootFloorDistance = (LeftFootPosition + FVector(0, 0, AutomaticHeightOffset) - HitLeftFoot.ImpactPoint).Z;
			}

			if (RaycastRightFoot)
			{
				RightFootFloorDistance = (RightFootPosition + FVector(0, 0, AutomaticHeightOffset) - HitRightFoot.ImpactPoint).Z;
			}

			if (abs(fminf(LeftFootFloorDistance, RightFootFloorDistance)) <= DistanceToFloorThreshold)
			{
				// Reset counter 
				DurationOffsetError = 0;
			}
			else
			{
				auto NowTS_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				DurationOffsetError += (NowTS_ms - PreviousTS_ms) / 1000.0f;
				PreviousTS_ms = NowTS_ms;

				if (DurationOffsetError > DurationOffsetErrorThreshold)
				{
					AutomaticHeightOffset = fmin(LeftFootFloorDistance, RightFootFloorDistance);
					DurationOffsetError = 0;

					//UE_LOG(LogTemp, Warning, TEXT("Recomputing offset ... %f"), AutomaticHeightOffset);
				}
			}
		}
	}
	else
	{
		AutomaticHeightOffset = 0;
	}

	TArray<FName> TargetBoneNames;
	SkeletalMesh->GetBoneNames(TargetBoneNames);

	for (auto& TargetBoneName : TargetBoneNames)
	{
		const FName* SourceBoneName = BoneNameMap.FindKey(TargetBoneName);

		if (SourceBoneName && !SourceBoneName->IsEqual("PELVIS")) // Do not scale the root
		{
			int SourceBoneID = *Keypoints.FindKey(*SourceBoneName);
			FVector BonePosition = InFrameData->Transforms[SourceBoneID].GetLocation();
			FVector ParentSourceBonePosition = InFrameData->Transforms[ParentsIdx[SourceBoneID]].GetLocation();

			int ParentIdx = ParentsIdx[SourceBoneID];

			FName ParentSourceBoneName = Keypoints[ParentIdx];
			FName ParentTargetBoneName = BoneNameMap[ParentSourceBoneName];
			float BoneSize = (ParentSourceBonePosition - BonePosition).Size();

			// Store the size of each bone of the ref pose.
			ZEDBoneSize.Add(ParentTargetBoneName, BoneSize);

			// Only do it once
			if (!RefPoseBoneSize.Contains(ParentTargetBoneName)) 
			{
				FVector TargetBonePosition = SkeletalMesh->GetBoneLocation(TargetBoneName, EBoneSpaces::WorldSpace);
				FVector ParentTargetBonePosition = SkeletalMesh->GetBoneLocation(ParentTargetBoneName, EBoneSpaces::WorldSpace);

				BoneSize = FVector::Distance(TargetBonePosition, ParentTargetBonePosition);

				// Store the size of each bone of the ref pose.
				RefPoseBoneSize.Add(ParentTargetBoneName, BoneSize);

			}
		}
	}

    putInRefPose(OutPose, TransformedBoneNames);
    FCompactPoseBoneIndex CPIndexRoot = GetCPIndex(0, OutPose, TransformedBoneNames);

	if (CPIndexRoot != INDEX_NONE)
        propagateRestPoseRotations(0, OutPose, TransformedBoneNames, SourceBoneParents, OutPose.GetRefPose(CPIndexRoot).GetRotation(), false);
	
    // Iterate over remapped bone names, find the index of that bone on the skeleton, and apply the Live Link pose data.
    for (int32 i = 0; i < TransformedBoneNames.Num(); i++)
    {
        FName BoneName = TransformedBoneNames[i];  

        if (!BoneName.ToString().ToLower().Contains("conf")) { // ignore kp confidence stored as kp
            FTransform BoneTransform = InFrameData->Transforms[i];
            FCompactPoseBoneIndex CPIndex = GetCPIndex(i, OutPose, TransformedBoneNames);
            if (CPIndex != INDEX_NONE)
            {
                FQuat Rotation;
                FVector Translation;

                // Only use position + rotation data for root. For all other bones, set rotation only.
                if (BoneName == BoneNameMap[GetTargetRootName()])
                {
					float rootScaleFactor = ComputeRootTranslationFactor(OutPose, TransformedBoneNames, InFrameData);

                    FVector RootPosition = BoneTransform.GetTranslation();
                    FCompactPoseBoneIndex leftUpLegIndex = GetCPIndex(*Keypoints.FindKey(FName("LEFT_HIP")), OutPose, TransformedBoneNames);
                    float HipOffset = FMath::Abs(OutPose[leftUpLegIndex].GetTranslation().Z) * OutPose[CPIndexRoot].GetScale3D().Z;

                    RootPosition.Z += HipOffset; // The position of the root in UE and in the SDK are slightly different. This offset compensates it.
					RootPosition.Z += ManualHeightOffset;
					RootPosition.Z -= AutomaticHeightOffset;
					//RootPosition.Z *= rootScaleFactor;

					Translation = RootPosition;

					Rotation = BoneTransform.GetRotation();
                }
                else
                {
					Rotation = BoneTransform.GetRotation();
                    Translation = OutPose[CPIndex].GetTranslation();
                }

                // Retrieves the default reference pose for the skeleton. Live Link data contains relative transforms from the default pose.
                FQuat FinalRotation = Rotation * OutPose[CPIndex].GetRotation();
                OutPose[CPIndex].SetRotation(FinalRotation);
                OutPose[CPIndex].SetTranslation(Translation);
            }
        }
    }

	if (bBoneScalingEnabled)
	{
		for (auto& TargetBoneName : TargetBoneNames)
		{
			if (ZEDBoneSize.Find(TargetBoneName) && RefPoseBoneSize.Find(TargetBoneName) && RefPoseBoneSize[TargetBoneName] != 0)
			{
				if (!BonesScale.Contains(TargetBoneName)) BonesScale.Add(TargetBoneName, FVector::OneVector);

				FName SourceBoneName = *BoneNameMap.FindKey(TargetBoneName);
				int SourceBoneID = *Keypoints.FindKey(SourceBoneName);
				float SourceBoneConfidence = InFrameData->Transforms[SourceBoneID + NbKeypoints / 2].GetLocation().X;

				float BoneScale = 1;
				float ParentBoneScale = 1;
				FVector FinalScale = FVector::OneVector;


				if (Keypoints.FindKey(SourceBoneName) && Keypoints.Find(ParentsIdx[*Keypoints.FindKey(SourceBoneName)]))
				{
					if (InFrameData->Transforms[*Keypoints.FindKey(SourceBoneName) + NbKeypoints / 2].GetLocation().X > 90)
					{
						FName TargetParentBoneName = BoneNameMap[Keypoints[ParentsIdx[SourceBoneID]]];

						BoneScale = ZEDBoneSize[TargetBoneName] / RefPoseBoneSize[TargetBoneName];
						ParentBoneScale = ZEDBoneSize[TargetParentBoneName] / RefPoseBoneSize[TargetParentBoneName];

						FinalScale = BoneScaleAlpha * BonesScale[TargetBoneName] + (1 - BoneScaleAlpha) * FVector(1, 1, BoneScale);
					}
					else
					{
						FinalScale = BonesScale[TargetBoneName];
					}
				}

				BonesScale.Emplace(TargetBoneName, FinalScale);
				int idx = *Keypoints.FindKey(SourceBoneName);
				FCompactPoseBoneIndex CPIndex = GetCPIndex(idx, OutPose, TransformedBoneNames);

				OutPose[CPIndex].SetScale3D(FinalScale);
			}
		}
	}

    if (CPIndexRoot != INDEX_NONE)
        propagateRestPoseRotations(0, OutPose, TransformedBoneNames, SourceBoneParents, OutPose.GetRefPose(CPIndexRoot).GetRotation().Inverse(), true);
}
