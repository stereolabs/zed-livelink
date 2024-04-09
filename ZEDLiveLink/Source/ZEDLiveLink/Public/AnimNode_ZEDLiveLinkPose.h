// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimNodeBase.h"
#include "Roles/LiveLinkAnimationTypes.h"
#include "LiveLinkTypes.h"
#include "ILiveLinkClient.h"

#include "CoreMinimal.h"
#include <deque>
#include <algorithm>

#include "AnimNode_ZEDLiveLinkPose.generated.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

USTRUCT(BlueprintType)
struct ZEDLIVELINK_API FAnimNode_ZEDLiveLinkPose: public FAnimNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FPoseLink InputPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	FLiveLinkSubjectName LiveLinkSubjectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	TMap<FName, FName> BoneNameMap34;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	TMap<FName, FName> BoneNameMap38;

	TMap<FName, FName>* CurBoneNameMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	bool bMirrorOnZAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	float ManualHeightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	bool bStickAvatarOnFloor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceData, meta = (PinShownByDefault))
	bool bEnableScaling;

public:
	FAnimNode_ZEDLiveLinkPose();

	//~ FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext & Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext & Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual bool HasPreUpdate() const { return true; }
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;
	//~ End of FAnimNode_Base interface

	bool Serialize(FArchive& Ar);
protected:
	virtual void OnInitializeAnimInstance(const FAnimInstanceProxy* InProxy, const UAnimInstance* InAnimInstance) override;

private:
	void PropagateRestPoseRotations(int32 parentIdx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, TArray<int32> SourceBoneParents, FQuat restPoseRot, bool inverse);

	void BuildPoseFromZEDAnimationData(float DeltaTime, const FLiveLinkSkeletonStaticData* InSkeletonData,
		const FLiveLinkAnimationFrameData* InFrameData,
		FCompactPose& OutPose);
	void PutInRefPose(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames);
	FCompactPoseBoneIndex GetCPIndex(int32 idx, FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames);
	float ComputeRootTranslationFactor(FCompactPose& OutPose, TArray<FName, TMemStackAllocator<>> TransformedBoneNames, const FLiveLinkAnimationFrameData* InFrameData);

	// This is the bone we will apply position translation to.
	// The root in our case is the pelvis (0)
	FName GetTargetRootName() const { return "PELVIS"; }

	ILiveLinkClient* LiveLinkClient_AnyThread;

	// Delta time from update so that it can be passed to retargeter
	float CachedDeltaTime;

    int NbKeypoints = -1;
    TMap<int, FName> Keypoints;
    TMap<int, FName> KeypointsMirrored;
    TArray<int> ParentsIdx;

	//FBoneContainer& RequiredBones;

    float DurationOffsetErrorThreshold = 3.0f;
    float DurationOffsetError = 0.0f;
    long long PreviousTS_ms = 0;

    float DistanceToFloorThreshold = 1.0f; // cm

	float AutomaticHeightOffset = 0;
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS

template<> struct TStructOpsTypeTraits<FAnimNode_ZEDLiveLinkPose> : public TStructOpsTypeTraitsBase2<FAnimNode_ZEDLiveLinkPose>
{
	enum
	{
		WithSerializer = true
	};
};


