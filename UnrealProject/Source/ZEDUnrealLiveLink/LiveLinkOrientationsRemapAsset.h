#pragma once

#include "CoreMinimal.h"
#include "LiveLinkRemapAsset.h"

#include "Containers/CircularQueue.h"
#include <deque>
#include <algorithm>

#include "LiveLinkOrientationsRemapAsset.generated.h"

/**
 *
 */
UCLASS()
class ZEDUNREALLIVELINK_API ULiveLinkOrientationsRemapAsset : public ULiveLinkRemapAsset
{
	GENERATED_BODY()

    void propagateRestPoseRotations(int32 parentIdx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, TArray<int32> SourceBoneParents, FQuat restPoseRot, bool inverse);
    void putInRefPose(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames);
    FCompactPoseBoneIndex GetCPIndex(int32 idx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames);

public:
    void BuildPoseFromAnimationData(float DeltaTime, const FLiveLinkSkeletonStaticData* InSkeletonData,
        const FLiveLinkAnimationFrameData* InFrameData,
        FCompactPose& OutPose, USkeletalMeshComponent* SkeletalMesh);

	void EnableStickAvatarOnFloor(bool bEnableStickAvatarOnFloor);
	void SetHeightOffset(float HeightOffset);
	void EnableBoneScaling(bool bBoneScalingEnabled);

protected:
        // This is the bone we will apply position translation to.
        // The root in our case is the pelvis
        virtual FName GetTargetRootName() const { return "PELVIS"; }

		int NbKeypoints = -1;
		TMap<int, FName> Keypoints;
		TArray<int> ParentsIdx;

        // Cached lookup results from GetRemappedBoneName
        TMap<FName, FName> BoneNameMap;

		TMap<FName, float> RefPoseBoneSize;
		TMap<FName, float> ZEDBoneSize;

		TMap<FName, FVector> BonesScale;

        // factor used to computer foot offset over time.
        float BoneScaleAlpha = 0.2f;
		
		int FeetOffsetBufferSize = 120;
		std::deque<float> FeetOffsetBuffer;
		float FeetOffsetAlpha = 1.0f;

		float FeetOffset = 0;
		float HeightOffset = 0;

		bool bStickAvatarOnFloor = true;

		bool bBoneScalingEnabled;

};
