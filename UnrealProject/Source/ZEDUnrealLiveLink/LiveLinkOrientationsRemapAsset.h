// Fill out your copyright notice in the Description page of Project Settings.

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

    virtual void BuildPoseFromAnimationData(float DeltaTime, const FLiveLinkSkeletonStaticData* InSkeletonData,
        const FLiveLinkAnimationFrameData* InFrameData,
        FCompactPose& OutPose) override;
    float ComputeRootTranslationFactor(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, const FLiveLinkAnimationFrameData* InFrameData);
    void propagateRestPoseRotations(int32 parentIdx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, TArray<int32> SourceBoneParents, FQuat restPoseRot, bool inverse);
    void putInRefPose(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames);
    FCompactPoseBoneIndex getCPIndex(int32 idx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames);

    protected:
        // Override these in child classes
        virtual FVector ConvertRootPosition(FVector LLPosition) const { return LLPosition; };
        virtual FQuat ConvertRootRotation(FQuat LLRotation) const { return LLRotation; };
        virtual FQuat ConvertBoneRotation(FQuat LLRotation) const { return LLRotation; };

        // This is the bone we will apply position translation to.
        // The root in our case is the pelvis
        virtual FName GetTargetRootName() const { return "PELVIS"; }

        // Cached lookup results from GetRemappedBoneName
        TMap<FName, FName> BoneNameMap;
	
};
