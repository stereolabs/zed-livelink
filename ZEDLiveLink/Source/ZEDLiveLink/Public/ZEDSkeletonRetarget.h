#ifndef __ZEDSKELETONRETARGET_H__
#define __ZEDSKELETONRETARGET_H__

static TArray<int> Parents34Idx = TArray<int>{
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
		24,
		//confidences
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

const TArray<int> Parents38Idx = {
	-1,
	0,
	1,
	2,
	3,
	4,
	4,
	4,
	4,
	4,
	3,
	3,
	10,
	11,
	12,
	13,
	14,
	15,
	0,
	0,
	18,
	19,
	20,
	21,
	22,
	23,
	22,
	23,
	22,
	23,
	16,
	17,
	16,
	17,
	16,
	17,
	16,
	17,
	// confidences
	-1,
	0,
	1,
	2,
	3,
	4,
	4,
	4,
	4,
	4,
	3,
	3,
	10,
	11,
	12,
	13,
	14,
	15,
	0,
	0,
	18,
	19,
	20,
	21,
	22,
	23,
	22,
	23,
	22,
	23,
	16,
	17,
	16,
	17,
	16,
	17,
	16,
	17
};

const TArray<FName> TargetBone34 = {
	"PELVIS",
	"NAVAL_SPINE",
	"CHEST_SPINE",
	"NECK",
	"LEFT_CLAVICLE",
	"LEFT_SHOULDER",
	"LEFT_ELBOW",
	"LEFT_WRIST",
	"LEFT_HAND",
	"LEFT_HANDTIP",
	"LEFT_THUMB",
	"RIGHT_CLAVICLE",
	"RIGHT_SHOULDER",
	"RIGHT_ELBOW",
	"RIGHT_WRIST",
	"RIGHT_HAND",
	"RIGHT_HANDTIP",
	"RIGHT_THUMB",
	"LEFT_HIP",
	"LEFT_KNEE",
	"LEFT_ANKLE",
	"LEFT_FOOT",
	"RIGHT_HIP",
	"RIGHT_KNEE",
	"RIGHT_ANKLE",
	"RIGHT_FOOT",
	"HEAD",
	"NOSE",
	"LEFT_EYE",
	"LEFT_EAR",
	"RIGHT_EYE",
	"RIGHT_EAR",
	"LEFT_HEEL",
	"RIGHT_HEEL",
	"CONF_PELVIS",
	"CONF_NAVAL_SPINE",
	"CONF_CHEST_SPINE",
	"CONF_NECK",
	"CONF_LEFT_CLAVICLE",
	"CONF_LEFT_SHOULDER",
	"CONF_LEFT_ELBOW",
	"CONF_LEFT_WRIST",
	"CONF_LEFT_HAND",
	"CONF_LEFT_HANDTIP",
	"CONF_LEFT_THUMB",
	"CONF_RIGHT_CLAVICLE",
	"CONF_RIGHT_SHOULDER",
	"CONF_RIGHT_ELBOW",
	"CONF_RIGHT_WRIST",
	"CONF_RIGHT_HAND",
	"CONF_RIGHT_HANDTIP",
	"CONF_RIGHT_THUMB",
	"CONF_LEFT_HIP",
	"CONF_LEFT_KNEE",
	"CONF_LEFT_ANKLE",
	"CONF_LEFT_FOOT",
	"CONF_RIGHT_HIP",
	"CONF_RIGHT_KNEE",
	"CONF_RIGHT_ANKLE",
	"CONF_RIGHT_FOOT",
	"CONF_HEAD",
	"CONF_NOSE",
	"CONF_LEFT_EYE",
	"CONF_LEFT_EAR",
	"CONF_RIGHT_EYE",
	"CONF_RIGHT_EAR",
	"CONF_LEFT_HEEL",
	"CONF_RIGHT_HEEL"
};

static TMap<int, FName> Keypoints38 = TMap<int, FName>{
	{0, "PELVIS"},
	{1, "SPINE_1"},
	{2, "SPINE_2"},
	{3, "SPINE_3"},
	{4, "NECK"},
	{5, "NOSE"},
	{6, "LEFT_EYE"},
	{7, "RIGHT_EYE"},
	{8, "LEFT_EAR"},
	{9, "RIGHT_EAR"},
	{10, "LEFT_CLAVICLE"},
	{11, "RIGHT_CLAVICLE"},
	{12, "LEFT_SHOULDER"},
	{13, "RIGHT_SHOULDER"},
	{14, "LEFT_ELBOW"},
	{15, "RIGHT_ELBOW"},
	{16, "LEFT_WRIST"},
	{17, "RIGHT_WRIST"},
	{18, "LEFT_HIP"},
	{19, "RIGHT_HIP"},
	{20, "LEFT_KNEE"},
	{21, "RIGHT_KNEE"},
	{22, "LEFT_ANKLE"},
	{23, "RIGHT_ANKLE"},
	{24, "LEFT_BIG_TOE"},
	{25, "RIGHT_BIG_TOE"},
	{26, "LEFT_SMALL_TOE"},
	{27, "RIGHT_SMALL_TOE"},
	{28, "LEFT_HEEL"},
	{29, "RIGHT_HEEL"},
	{30, "LEFT_HAND_THUMB_4"},
	{31, "RIGHT_HAND_THUMB_4"},
	{32, "LEFT_HAND_INDEX_1"},
	{33, "RIGHT_HAND_INDEX_1"},
	{34, "LEFT_HAND_MIDDLE_4"},
	{35, "RIGHT_HAND_MIDDLE_4"},
	{36, "LEFT_HAND_PINKY_1"},
	{37, "RIGHT_HAND_PINKY_1"},
};

const TArray<FName> TargetBone38 = {
	"PELVIS",
	"SPINE_1",
	"SPINE_2",
	"SPINE_3",
	"NECK",
	"NOSE",
	"LEFT_EYE",
	"RIGHT_EYE",
	"LEFT_EAR",
	"RIGHT_EAR",
	"LEFT_CLAVICLE",
	"RIGHT_CLAVICLE",
	"LEFT_SHOULDER",
	"RIGHT_SHOULDER",
	"LEFT_ELBOW",
	"RIGHT_ELBOW",
	"LEFT_WRIST",
	"RIGHT_WRIST",
	"LEFT_HIP",
	"RIGHT_HIP",
	"LEFT_KNEE",
	"RIGHT_KNEE",
	"LEFT_ANKLE",
	"RIGHT_ANKLE",
	"LEFT_BIG_TOE",
	"RIGHT_BIG_TOE",
	"LEFT_SMALL_TOE",
	"RIGHT_SMALL_TOE",
	"LEFT_HEEL",
	"RIGHT_HEEL",
	"LEFT_HAND_THUMB_4",
	"RIGHT_HAND_THUMB_4",
	"LEFT_HAND_INDEX_1",
	"RIGHT_HAND_INDEX_1",
	"LEFT_HAND_MIDDLE_4",
	"RIGHT_HAND_MIDDLE_4",
	"LEFT_HAND_PINKY_1",
	"RIGHT_HAND_PINKY_1",
	// Confidences
	"CONF_PELVIS",
	"CONF_SPINE_1",
	"CONF_SPINE_2",
	"CONF_SPINE_3",
	"CONF_NECK",
	"CONF_NOSE",
	"CONF_LEFT_EYE",
	"CONF_RIGHT_EYE",
	"CONF_LEFT_EAR",
	"CONF_RIGHT_EAR",
	"CONF_LEFT_CLAVICLE",
	"CONF_RIGHT_CLAVICLE",
	"CONF_LEFT_SHOULDER",
	"CONF_RIGHT_SHOULDER",
	"CONF_LEFT_ELBOW",
	"CONF_RIGHT_ELBOW",
	"CONF_LEFT_WRIST",
	"CONF_RIGHT_WRIST",
	"CONF_LEFT_HIP",
	"CONF_RIGHT_HIP",
	"CONF_LEFT_KNEE",
	"CONF_RIGHT_KNEE",
	"CONF_LEFT_ANKLE",
	"CONF_RIGHT_ANKLE",
	"CONF_LEFT_BIG_TOE",
	"CONF_RIGHT_BIG_TOE",
	"CONF_LEFT_SMALL_TOE",
	"CONF_RIGHT_SMALL_TOE",
	"CONF_LEFT_HEEL",
	"CONF_RIGHT_HEEL",
	"CONF_LEFT_HAND_THUMB_4",
	"CONF_RIGHT_HAND_THUMB_4",
	"CONF_LEFT_HAND_INDEX_1",
	"CONF_RIGHT_HAND_INDEX_1",
	"CONF_LEFT_HAND_MIDDLE_4",
	"CONF_RIGHT_HAND_MIDDLE_4",
	"CONF_LEFT_HAND_PINKY_1",
	"CONF_RIGHT_HAND_PINKY_1"
};

static TMap<int, FName> Keypoints34 = TMap<int, FName>{
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

static TMap<int, FName> Keypoints34Mirrored = TMap<int, FName>{
	{0, "PELVIS"},
	{1, "NAVAL_SPINE"},
	{2, "CHEST_SPINE"},
	{3, "NECK"},
	{4, "RIGHT_CLAVICLE"},
	{5, "RIGHT_SHOULDER"},
	{6, "RIGHT_ELBOW"},
	{7, "RIGHT_WRIST"},
	{8, "RIGHT_HAND"},
	{9, "RIGHT_HANDTIP"},
	{10, "RIGHT_THUMB"},
	{11, "LEFT_CLAVICLE"},
	{12, "LEFT_SHOULDER"},
	{13, "LEFT_ELBOW"},
	{14, "LEFT_WRIST"},
	{15, "LEFT_HAND"},
	{16, "LEFT_HANDTIP"},
	{17, "LEFT_THUMB"},
	{18, "RIGHT_HIP"},
	{19, "RIGHT_KNEE"},
	{20, "RIGHT_ANKLE"},
	{21, "RIGHT_FOOT"},
	{22, "LEFT_HIP"},
	{23, "LEFT_KNEE"},
	{24, "LEFT_ANKLE"},
	{25, "LEFT_FOOT"},
	{26, "HEAD"},
	{27, "NOSE"},
	{28, "RIGHT_EYE"},
	{29, "RIGHT_EAR"},
	{30, "LEFT_EYE"},
	{31, "LEFT_EAR"},
	{32, "RIGHT_HEEL"},
	{33, "LEFT_HEEL"}
};

static TMap<int, FName> Keypoints38Mirrored = TMap<int, FName>{
	 {0, "PELVIS"},
	{1, "SPINE_1"},
	{2, "SPINE_2"},
	{3, "SPINE_3"},
	{4, "NECK"},
	{5, "NOSE"},
	{6, "RIGHT_EYE"},
	{7, "LEFT_EYE"},
	{8, "RIGHT_EAR"},
	{9, "LEFT_EAR"},
	{10, "RIGHT_CLAVICLE"},
	{11, "LEFT_CLAVICLE"},
	{12, "RIGHT_SHOULDER"},
	{13, "LEFT_SHOULDER"},
	{14, "RIGHT_ELBOW"},
	{15, "LEFT_ELBOW"},
	{16, "RIGHT_WRIST"},
	{17, "LEFT_WRIST"},
	{18, "RIGHT_HIP"},
	{19, "LEFT_HIP"},
	{20, "RIGHT_KNEE"},
	{21, "LEFT_KNEE"},
	{22, "RIGHT_ANKLE"},
	{23, "LEFT_ANKLE"},
	{24, "RIGHT_BIG_TOE"},
	{25, "LEFT_BIG_TOE"},
	{26, "RIGHT_SMALL_TOE"},
	{27, "LEFT_SMALL_TOE"},
	{28, "RIGHT_HEEL"},
	{29, "LEFT_HEEL"},
	{30, "RIGHT_HAND_THUMB_4"},
	{31, "LEFT_HAND_THUMB_4"},
	{32, "RIGHT_HAND_INDEX_1"},
	{33, "LEFT_HAND_INDEX_1"},
	{34, "RIGHT_HAND_MIDDLE_4"},
	{35, "LEFT_HAND_MIDDLE_4"},
	{36, "RIGHT_HAND_PINKY_1"},
	{37, "LEFT_HAND_PINKY_1"},
};

static FName GetParent34BoneName(FName BoneName)
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

static FName GetParent38BoneName(FName BoneName)
{
	FName ParentBoneName = "None";

	if (BoneName.IsEqual("PELVIS"))
		ParentBoneName = "None";
	else if (BoneName.IsEqual("SPINE_1"))
		ParentBoneName = "PELVIS";
	else if (BoneName.IsEqual("SPINE_2"))
		ParentBoneName = "SPINE_1";
	else if (BoneName.IsEqual("SPINE_3"))
		ParentBoneName = "SPINE_2";
	else if (BoneName.IsEqual("NECK"))
		ParentBoneName = "SPINE_3";
	else if (BoneName.IsEqual("LEFT_CLAVICLE"))
		ParentBoneName = "SPINE_3";
	else if (BoneName.IsEqual("LEFT_SHOULDER"))
		ParentBoneName = "LEFT_CLAVICLE";
	else if (BoneName.IsEqual("LEFT_ELBOW"))
		ParentBoneName = "LEFT_SHOULDER";
	else if (BoneName.IsEqual("LEFT_WRIST"))
		ParentBoneName = "LEFT_ELBOW";
	else if (BoneName.IsEqual("LEFT_HAND_THUMB_4"))
		ParentBoneName = "LEFT_WRIST";
	else if (BoneName.IsEqual("LEFT_HAND_INDEX_1"))
		ParentBoneName = "LEFT_WRIST";
	else if (BoneName.IsEqual("LEFT_HAND_MIDDLE_4"))
		ParentBoneName = "LEFT_WRIST";
	else if (BoneName.IsEqual("LEFT_HAND_PINKY_1"))
		ParentBoneName = "LEFT_WRIST";
	else if (BoneName.IsEqual("RIGHT_CLAVICLE"))
		ParentBoneName = "SPINE_3";
	else if (BoneName.IsEqual("RIGHT_SHOULDER"))
		ParentBoneName = "RIGHT_CLAVICLE";
	else if (BoneName.IsEqual("RIGHT_ELBOW"))
		ParentBoneName = "RIGHT_SHOULDER";
	else if (BoneName.IsEqual("RIGHT_WRIST"))
		ParentBoneName = "RIGHT_ELBOW";
	else if (BoneName.IsEqual("RIGHT_HAND_THUMB_4"))
		ParentBoneName = "RIGHT_WRIST";
	else if (BoneName.IsEqual("RIGHT_HAND_INDEX_1"))
		ParentBoneName = "RIGHT_WRIST";
	else if (BoneName.IsEqual("RIGHT_HAND_MIDDLE_4"))
		ParentBoneName = "RIGHT_WRIST";
	else if (BoneName.IsEqual("RIGHT_HAND_PINKY_1"))
		ParentBoneName = "RIGHT_WRIST";
	else if (BoneName.IsEqual("LEFT_HIP"))
		ParentBoneName = "PELVIS";
	else if (BoneName.IsEqual("LEFT_KNEE"))
		ParentBoneName = "LEFT_HIP";
	else if (BoneName.IsEqual("LEFT_ANKLE"))
		ParentBoneName = "LEFT_KNEE";
	else if (BoneName.IsEqual("LEFT_BIG_TOE"))
		ParentBoneName = "LEFT_ANKLE";
	else if (BoneName.IsEqual("LEFT_SMALL_TOE"))
		ParentBoneName = "LEFT_ANKLE";
	else if (BoneName.IsEqual("RIGHT_HIP"))
		ParentBoneName = "PELVIS";
	else if (BoneName.IsEqual("RIGHT_KNEE"))
		ParentBoneName = "RIGHT_HIP";
	else if (BoneName.IsEqual("RIGHT_ANKLE"))
		ParentBoneName = "RIGHT_KNEE";
	else if (BoneName.IsEqual("RIGHT_BIG_TOE"))
		ParentBoneName = "RIGHT_ANKLE";
	else if (BoneName.IsEqual("RIGHT_SMALL_TOE"))
		ParentBoneName = "RIGHT_ANKLE";
	else if (BoneName.IsEqual("NOSE"))
		ParentBoneName = "NECK";
	else if (BoneName.IsEqual("LEFT_EYE"))
		ParentBoneName = "NECK";
	else if (BoneName.IsEqual("RIGHT_EYE"))
		ParentBoneName = "NECK";
	else if (BoneName.IsEqual("LEFT_EAR"))
		ParentBoneName = "LEFT_EYE";
	else if (BoneName.IsEqual("RIGHT_EAR"))
		ParentBoneName = "RIGHT_EYE";
	else if (BoneName.IsEqual("LEFT_HEEL"))
		ParentBoneName = "LEFT_ANKLE";
	else if (BoneName.IsEqual("RIGHT_HEEL"))
		ParentBoneName = "RIGHT_ANKLE";

	return ParentBoneName;
}

static FName GetParentBoneName(FName BoneName, int NbKeypoints = 34)
{
	if (NbKeypoints == 34)
	{
		return GetParent34BoneName(BoneName);
	}
	else if (NbKeypoints == 38)
	{
		return GetParent38BoneName(BoneName);
	}
	else
	{
		return GetParent38BoneName(BoneName);
	}
}
#endif