#pragma once

#include "CoreMinimal.h"
#include "LiveLinkRemapAsset.h"

#include "Containers/CircularQueue.h"
#include "LiveLinkOrientationsRemapAsset.generated.h"

#include <deque>
#include <algorithm>

/**
 *
 */
UCLASS()
class ZEDUNREALLIVELINK_API ULiveLinkOrientationsRemapAsset : public ULiveLinkRemapAsset
{
	GENERATED_BODY()

    float ComputeRootTranslationFactor(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, const FLiveLinkAnimationFrameData* InFrameData);
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

        // Cached lookup results from GetRemappedBoneName
        TMap<FName, FName> BoneNameMap;

		TMap<FName, float> RefPoseBoneSize;
		TMap<FName, float> ZEDBoneSize;

		TMap<FName, FVector> BonesScale;
		float RefPoseChestLength;

        // factor used to computer foot offset over time.
        float BoneScaleAlpha = 0.2f;
		
		int FeetOffsetBufferSize = 200;
		std::deque<float> FeetOffsetBuffer;

		float FeetOffset = 0;
		float HeightOffset = 0;

		bool bStickAvatarOnFloor = true;

		bool bBoneScalingEnabled;

};
