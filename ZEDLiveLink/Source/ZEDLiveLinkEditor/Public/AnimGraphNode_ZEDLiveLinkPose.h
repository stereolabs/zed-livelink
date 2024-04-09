#pragma once

#include "AnimGraphNode_Base.h"
#include "AnimNode_ZEDLiveLinkPose.h"
#include "AnimGraphDefinitions.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "AnimGraphNode_ZEDLiveLinkPose.generated.h"

UCLASS(BlueprintType)
class ZEDLIVELINKEDITOR_API UAnimGraphNode_ZEDLiveLinkPose : public UAnimGraphNode_Base
{
	GENERATED_BODY()

public:

	//~ UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetMenuCategory() const;
	//~ End of UEdGraphNode

public:

	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_ZEDLiveLinkPose Node;

};
