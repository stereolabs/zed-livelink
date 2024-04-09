#include "AnimGraphNode_ZEDLiveLinkPose.h"
#include "EdGraph/EdGraphSchema.h"
#include "Animation/AnimAttributes.h"

#define LOCTEXT_NAMESPACE "ZEDAnimNode"

FText UAnimGraphNode_ZEDLiveLinkPose::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "ZED LiveLinkPose");
}

FText UAnimGraphNode_ZEDLiveLinkPose::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Retrieves the current pose associated with the supplied detection");
}

FText UAnimGraphNode_ZEDLiveLinkPose::GetMenuCategory() const
{
	return LOCTEXT("NodeCategory", "ZED");
}


#undef LOCTEXT_NAMESPACE
