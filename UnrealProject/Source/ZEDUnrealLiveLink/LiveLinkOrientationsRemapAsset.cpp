#include "LiveLinkOrientationsRemapAsset.h"

#include "Roles/LiveLinkAnimationTypes.h"
#include "BonePose.h"

static TArray<int> ParentsIdx = TArray<int>{
	   -1,
		0,
		1,
		2,
		2,
		4,
		5,
		6,
		7,
		8,
		7,
		2,
		11,
		12,
		13,
		14,
		15,
		14,
		0,
		18,
		19,
		20,
		0,
		22,
		23,
		24,
		3,
		26,
		26,
		26,
		26,
		26,
		20,
		24
};

static TMap<int, FName> Keypoints = TMap<int, FName>{
	{0, "PELVIS"},
	{1, "NAVAL_SPINE"},
	{2, "CHEST_SPINE"},
	{3, "NECK"},
	{4, "LEFT_CLAVICLE"},
	{5, "LEFT_SHOULDER"},
	{6, "LEFT_ELBOW"},
	{7, "LEFT_WRIST"},
	{8, "LEFT_HAND"},
	{9, "LEFT_HANDTIP"},
	{10, "LEFT_THUMB"},
	{11, "RIGHT_CLAVICLE"},
	{12, "RIGHT_SHOULDER"},
	{13, "RIGHT_ELBOW"},
	{14, "RIGHT_WRIST"},
	{15, "RIGHT_HAND"},
	{16, "RIGHT_HANDTIP"},
	{17, "RIGHT_THUMB"},
	{18, "LEFT_HIP"},
	{19, "LEFT_KNEE"},
	{20, "LEFT_ANKLE"},
	{21, "LEFT_FOOT"},
	{22, "RIGHT_HIP"},
	{23, "RIGHT_KNEE"},
	{24, "RIGHT_ANKLE"},
	{25, "RIGHT_FOOT"},
	{26, "HEAD"},
	{27, "NOSE"},
	{28, "LEFT_EYE"},
	{29, "LEFT_EAR"},
	{30, "RIGHT_EYE"},
	{31, "RIGHT_EAR"},
	{32, "LEFT_HEEL"},
	{33, "RIGHT_HEEL"}
};

static FName GetParentBoneName(FName BoneName)
{
	FName ParentBoneName = "None";

	if (BoneName.IsEqual("PELVIS"))
		ParentBoneName = "None";
	else if (BoneName.IsEqual("NAVAL_SPINE"))
		ParentBoneName = "PELVIS";
	else if (BoneName.IsEqual("CHEST_SPINE"))
		ParentBoneName = "NAVAL_SPINE";
	else if (BoneName.IsEqual("NECK"))
		ParentBoneName = "CHEST_SPINE";
	else if (BoneName.IsEqual("LEFT_CLAVICLE"))
		ParentBoneName = "CHEST_SPINE";
	else if (BoneName.IsEqual("LEFT_SHOULDER"))
		ParentBoneName = "LEFT_CLAVICLE";
	else if (BoneName.IsEqual("LEFT_ELBOW"))
		ParentBoneName = "LEFT_SHOULDER";
	else if (BoneName.IsEqual("LEFT_WRIST"))
		ParentBoneName = "LEFT_ELBOW";
	else if (BoneName.IsEqual("LEFT_HAND"))
		ParentBoneName = "LEFT_WRIST";
	else if (BoneName.IsEqual("LEFT_HANDTIP"))
		ParentBoneName = "LEFT_HAND";
	else if (BoneName.IsEqual("LEFT_THUMB"))
		ParentBoneName = "LEFT_WRIST";
	else if (BoneName.IsEqual("RIGHT_CLAVICLE"))
		ParentBoneName = "CHEST_SPINE";
	else if (BoneName.IsEqual("RIGHT_SHOULDER"))
		ParentBoneName = "RIGHT_CLAVICLE";
	else if (BoneName.IsEqual("RIGHT_ELBOW"))
		ParentBoneName = "RIGHT_SHOULDER";
	else if (BoneName.IsEqual("RIGHT_WRIST"))
		ParentBoneName = "RIGHT_ELBOW";
	else if (BoneName.IsEqual("RIGHT_HAND"))
		ParentBoneName = "RIGHT_WRIST";
	else if (BoneName.IsEqual("RIGHT_HANDTIP"))
		ParentBoneName = "RIGHT_HAND";
	else if (BoneName.IsEqual("RIGHT_THUMB"))
		ParentBoneName = "RIGHT_WRIST";
	else if (BoneName.IsEqual("LEFT_HIP"))
		ParentBoneName = "PELVIS";
	else if (BoneName.IsEqual("LEFT_KNEE"))
		ParentBoneName = "LEFT_HIP";
	else if (BoneName.IsEqual("LEFT_ANKLE"))
		ParentBoneName = "LEFT_KNEE";
	else if (BoneName.IsEqual("LEFT_FOOT"))
		ParentBoneName = "LEFT_ANKLE";
	else if (BoneName.IsEqual("RIGHT_HIP"))
		ParentBoneName = "PELVIS";
	else if (BoneName.IsEqual("RIGHT_KNEE"))
		ParentBoneName = "RIGHT_HIP";
	else if (BoneName.IsEqual("RIGHT_ANKLE"))
		ParentBoneName = "RIGHT_KNEE";
	else if (BoneName.IsEqual("RIGHT_FOOT"))
		ParentBoneName = "RIGHT_ANKLE";
	else if (BoneName.IsEqual("HEAD"))
		ParentBoneName = "NECK";
	else if (BoneName.IsEqual("NOSE"))
		ParentBoneName = "HEAD";
	else if (BoneName.IsEqual("LEFT_EYE"))
		ParentBoneName = "HEAD";
	else if (BoneName.IsEqual("RIGHT_EYE"))
		ParentBoneName = "HEAD";
	else if (BoneName.IsEqual("LEFT_EAR"))
		ParentBoneName = "HEAD";
	else if (BoneName.IsEqual("RIGHT_EAR"))
		ParentBoneName = "HEAD";
	else if (BoneName.IsEqual("LEFT_HEEL"))
		ParentBoneName = "LEFT_ANKLE";
	else if (BoneName.IsEqual("RIGHT_HEEL"))
		ParentBoneName = "RIGHT_ANKLE";

	return ParentBoneName;
}

void ULiveLinkOrientationsRemapAsset::SetHeightOffset(float Offset)
{
	HeightOffset = Offset;
}

void ULiveLinkOrientationsRemapAsset::EnableBoneScaling(bool BoneScaling)
{
	bBoneScalingEnabled = BoneScaling;
}

void ULiveLinkOrientationsRemapAsset::EnableStickAvatarOnFloor(bool bEnableStickAvatarOnFloor)
{
	bStickAvatarOnFloor = bEnableStickAvatarOnFloor;
}

float ULiveLinkOrientationsRemapAsset::ComputeRootTranslationFactor(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, const FLiveLinkAnimationFrameData* InFrameData) {
    float avatarTotalTranslation = 0.f;
    float SDKTotalTranslation = 0.f;
    for (int32 i = 23; i < 25; i++)
    {
        FTransform BoneTransform = InFrameData->Transforms[i];
        FCompactPoseBoneIndex CPIndex = GetCPIndex(i, OutPose, TransformedBoneNames);
        if (CPIndex != INDEX_NONE)
        {
            avatarTotalTranslation += OutPose[CPIndex].GetTranslation().Size();
            SDKTotalTranslation += BoneTransform.GetTranslation().Size();
        }
    }

    float factor = avatarTotalTranslation / SDKTotalTranslation;
    float scale = 1.f;
    FCompactPoseBoneIndex CPIndexRoot = GetCPIndex(0, OutPose, TransformedBoneNames);
    if (CPIndexRoot != INDEX_NONE)
        scale = OutPose[CPIndexRoot].GetScale3D().Z;
	return FMath::Abs(scale * factor);
}

FCompactPoseBoneIndex ULiveLinkOrientationsRemapAsset::GetCPIndex(int32 idx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames){
    FName BoneName = TransformedBoneNames[idx];
    const int32 MeshIndex = OutPose.GetBoneContainer().GetPoseBoneIndexForBoneName(BoneName);
    if (MeshIndex != INDEX_NONE)
    {
        FCompactPoseBoneIndex CPIndex = OutPose.GetBoneContainer().MakeCompactPoseIndex(
            FMeshPoseBoneIndex(MeshIndex));
        return CPIndex;
    }
    return (FCompactPoseBoneIndex) INDEX_NONE;
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
void ULiveLinkOrientationsRemapAsset::BuildPoseFromAnimationData(float DeltaTime,
    const FLiveLinkSkeletonStaticData* InSkeletonData,
    const FLiveLinkAnimationFrameData* InFrameData,
    FCompactPose& OutPose,
    USkeletalMeshComponent* SkeletalMesh)
{
    const TArray<FName>& SourceBoneNames = InSkeletonData->BoneNames;
    const TArray<int32>& SourceBoneParents = InSkeletonData->BoneParents;

    TArray<FName, TMemStackAllocator<>> TransformedBoneNames;
    TransformedBoneNames.Reserve(SourceBoneNames.Num());

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
	if (bStickAvatarOnFloor && InFrameData->Transforms[34 + 20].GetLocation().X >90 && InFrameData->Transforms[34 + 24].GetLocation().X > 90) { //if both foot are visible/detected
		if (SkeletalMesh) {

				FVector LeftFootPosition = SkeletalMesh->GetBoneLocation(TransformedBoneNames[21]);
				FVector RightFootPosition = SkeletalMesh->GetBoneLocation(TransformedBoneNames[25]);

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
					LeftFootFloorDistance = (LeftFootPosition + FVector(0, 0, FeetOffset) - HitLeftFoot.ImpactPoint).Z;
				}

				if (RaycastRightFoot)
				{
					RightFootFloorDistance = (RightFootPosition + FVector(0, 0, FeetOffset) - HitRightFoot.ImpactPoint).Z;
				}

				float MinFootFloorDistance = 0;

				// If both feet are under the ground, use the max value instead of the min value.
				if (RightFootFloorDistance < 0 && LeftFootFloorDistance < 0) {

					MinFootFloorDistance = -1.0f * fmax(abs(RightFootFloorDistance), abs(LeftFootFloorDistance));
					FeetOffset = FeetOffsetAlpha * MinFootFloorDistance + (1 - FeetOffsetAlpha) * FeetOffset;
				}
				else if (RightFootFloorDistance > 0 && LeftFootFloorDistance > 0)
				{
					MinFootFloorDistance = fmin(abs(RightFootFloorDistance), abs(LeftFootFloorDistance));

					// The feet offset is added in the buffer of size "FeetOffsetBufferSize". If the buffer is already full, remove the oldest value (the first of the deque)
					if (FeetOffsetBuffer.size() == FeetOffsetBufferSize)
					{
						FeetOffsetBuffer.pop_front();
					}
					FeetOffsetBuffer.push_back(MinFootFloorDistance);

					// The feet offset is the min element of this deque (of size FeetOffsetBufferSize).
					FeetOffset = *std::min_element(FeetOffsetBuffer.begin(), FeetOffsetBuffer.end());
				}
				else
				{
					MinFootFloorDistance = fmin(RightFootFloorDistance, LeftFootFloorDistance);
					FeetOffset = FeetOffsetAlpha * MinFootFloorDistance + (1 - FeetOffsetAlpha) * FeetOffset;
				}
		}
	}
	else
	{
		FeetOffset = 0;
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

	FVector NeckPosition = SkeletalMesh->GetBoneLocation(BoneNameMap["NECK"], EBoneSpaces::WorldSpace);
	FVector PelvisPosition = (SkeletalMesh->GetBoneLocation(BoneNameMap["LEFT_HIP"], EBoneSpaces::WorldSpace) + SkeletalMesh->GetBoneLocation(BoneNameMap["RIGHT_HIP"], EBoneSpaces::WorldSpace)) / 2.0f;

	RefPoseChestLength = (NeckPosition - PelvisPosition).Size();

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
                    float rootTranslationFactor = ComputeRootTranslationFactor(OutPose, TransformedBoneNames, InFrameData);
                    FVector RootPosition = BoneTransform.GetTranslation();
                    FCompactPoseBoneIndex leftUpLegIndex = GetCPIndex(18, OutPose, TransformedBoneNames);
                    float HipOffset = FMath::Abs(OutPose[leftUpLegIndex].GetTranslation().Z) * OutPose[CPIndexRoot].GetScale3D().Z;

                    RootPosition.Z += HipOffset; // The position of the root in UE and in the SDK are slightly different. This offset compensates it.
					RootPosition.Z += HeightOffset;
					RootPosition.Z -= FeetOffset;

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
		FVector ZEDNeckPosition = InFrameData->Transforms[*Keypoints.FindKey("NECK")].GetLocation();
		FVector ZEDPelvisPosition = InFrameData->Transforms[*Keypoints.FindKey("PELVIS")].GetLocation();

		float ZEDChestLength = (ZEDNeckPosition - ZEDPelvisPosition).Size();

		for (auto& TargetBoneName : TargetBoneNames)
		{
			if (ZEDBoneSize.Find(TargetBoneName) && RefPoseBoneSize.Find(TargetBoneName) && RefPoseBoneSize[TargetBoneName] != 0)
			{
				if (!BonesScale.Contains(TargetBoneName)) BonesScale.Add(TargetBoneName, FVector::OneVector);

				FName SourceBoneName = *BoneNameMap.FindKey(TargetBoneName);
				int SourceBoneID = *Keypoints.FindKey(SourceBoneName);
				float SourceBoneConfidence = InFrameData->Transforms[SourceBoneID + 34].GetLocation().X;

				float BoneScale = 1;
				float ParentBoneScale = 1;
				FVector FinalScale = FVector::OneVector;

				if (TargetBoneName.IsEqual("Hips")) //
				{
					if (InFrameData->Transforms[*Keypoints.FindKey("NECK") + 34].GetLocation().X > 90 && InFrameData->Transforms[*Keypoints.FindKey("PELVIS") + 34].GetLocation().X > 90)
					{
						BoneScale = ZEDChestLength / RefPoseChestLength;
						FinalScale = BoneScaleAlpha * BonesScale[TargetBoneName] + (1 - BoneScaleAlpha) * FVector(BoneScale, BoneScale, BoneScale);
					}
					else
					{
						FinalScale = BonesScale[TargetBoneName];
					}
				}
				// Scale all the bones of the chest with the same scale
				else if (TargetBoneName.IsEqual("Head") || TargetBoneName.IsEqual("Spine") || TargetBoneName.IsEqual("Spine1") || TargetBoneName.IsEqual("Spine2")
					|| TargetBoneName.IsEqual("Neck") || TargetBoneName.IsEqual("LeftShoulder") || TargetBoneName.IsEqual("RightShoulder"))
				{
					FinalScale = FVector::OneVector;
				}
				else
				{
					if (Keypoints.FindKey(SourceBoneName) && Keypoints.Find(ParentsIdx[*Keypoints.FindKey(SourceBoneName)]))
					{
						if (InFrameData->Transforms[*Keypoints.FindKey(SourceBoneName) + 34].GetLocation().X > 90)
						{
							FName TargetParentBoneName = BoneNameMap[Keypoints[ParentsIdx[SourceBoneID]]];

							BoneScale = ZEDBoneSize[TargetBoneName] / RefPoseBoneSize[TargetBoneName];
							ParentBoneScale = ZEDBoneSize[TargetParentBoneName] / RefPoseBoneSize[TargetParentBoneName];

							if (!TargetBoneName.IsEqual("LeftUpLeg") && !TargetBoneName.IsEqual("RightUpLeg") && !TargetBoneName.IsEqual("LeftArm") && !TargetBoneName.IsEqual("RightArm"))
							{
								BoneScale /= ParentBoneScale;
							}

							FinalScale = BoneScaleAlpha * BonesScale[TargetBoneName] + (1 - BoneScaleAlpha) * FVector(1, 1, BoneScale);
						}
						else
						{
							FinalScale = BonesScale[TargetBoneName];
						}
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
