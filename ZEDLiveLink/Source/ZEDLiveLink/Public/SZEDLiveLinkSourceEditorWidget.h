#pragma once

#include <Widgets/SCompoundWidget.h>

#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "ZEDLiveLinkSettings.h"

DECLARE_DELEGATE_OneParam(FZEDLiveLinkOnSourceCreated, FZEDLiveLinkSettings);

class SZEDLiveLinkSourceEditorWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SZEDLiveLinkSourceEditorWidget){}
	SLATE_EVENT(FZEDLiveLinkOnSourceCreated, OnSourceCreated)
SLATE_END_ARGS()

void Construct(const FArguments& Args);

private:

	TSharedPtr<FString> GetConnectionTypeString() const;
	void OnConnectionTypeChanged(TSharedPtr<FString> Value, ESelectInfo::Type SelectInfo);

	FReply OnCreateClicked() const;

	TSharedPtr<SEditableTextBox> IpAddress;
	EZEDLiveLinkConnectionType ConnectionType = EZEDLiveLinkConnectionType::Multicast;
	TArray<TSharedPtr <FString>> ConnectionTypeStrings;

	FZEDLiveLinkOnSourceCreated OnSourceCreated;
};