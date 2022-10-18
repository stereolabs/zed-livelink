#pragma once

#include "CoreMinimal.h"
#include "LiveLinkRemapAsset.h"
#include "LiveLinkOrientationsRemapAsset.generated.h"

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
    FCompactPoseBoneIndex getCPIndex(int32 idx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames);

public:
    void BuildPoseFromAnimationData(float DeltaTime, const FLiveLinkSkeletonStaticData* InSkeletonData,
        const FLiveLinkAnimationFrameData* InFrameData,
        FCompactPose& OutPose, USkeletalMeshComponent* SkeletalMesh);

    protected:
        // This is the bone we will apply position translation to.
        // The root in our case is the pelvis
        virtual FName GetTargetRootName() const { return "PELVIS"; }

        // Cached lookup results from GetRemappedBoneName
        TMap<FName, FName> BoneNameMap;

        float FeetOffset = 0;
        // factor used to computer foot offset over time.
        float Alpha = 0.1f;
};
