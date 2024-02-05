#include "SZEDLiveLinkSourceEditorWidget.h"
#include <Widgets/Text/STextBlock.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/SBoxPanel.h>
#include <Widgets/Input/STextComboBox.h>

#define LOCTEXT_NAMESPACE "ZEDLiveLinkSourceEditor"

void SZEDLiveLinkSourceEditorWidget::Construct(const FArguments& Args)
{
	OnSourceCreated = Args._OnSourceCreated;

	const float kRowPadding = 3;
	const float kLabelColMinWidth = 125;
	const float kEditColMinWidth = 125;

	ConnectionTypeStrings.Add(MakeShareable<FString>(new FString("Multicast")));
	ConnectionTypeStrings.Add(MakeShareable<FString>(new FString("Unicast")));

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.Padding(kRowPadding)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("IPAddressLabel", "IP Address"))
		.MinDesiredWidth(kLabelColMinWidth)
		]
	+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			SAssignNew(IpAddress, SEditableTextBox)
			.Text(FText::FromString("230.0.0.1:2000"))
		.MinDesiredWidth(kEditColMinWidth)
		]
		]
	+ SVerticalBox::Slot()
		.Padding(kRowPadding)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ConnectionTypeLabel", "Connection Type"))
		.MinDesiredWidth(kLabelColMinWidth)
		]
	+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			SNew(STextComboBox)
			.OptionsSource(&ConnectionTypeStrings)
		.InitiallySelectedItem(GetConnectionTypeString())
		.OnSelectionChanged(this, &SZEDLiveLinkSourceEditorWidget::OnConnectionTypeChanged)
		]
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 10, 0, 0)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.FillWidth(1)
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("CreateButtonLabel", "Create Source"))
		.HAlign(HAlign_Center)
		.OnClicked(this, &SZEDLiveLinkSourceEditorWidget::OnCreateClicked)
		]
	+ SHorizontalBox::Slot()
		.FillWidth(1)
		]
		];
}

TSharedPtr<FString> SZEDLiveLinkSourceEditorWidget::GetConnectionTypeString() const
{
	switch (ConnectionType)
	{
	case EZEDLiveLinkConnectionType::Unicast:
		return ConnectionTypeStrings[1];
	case EZEDLiveLinkConnectionType::Multicast:
	default:
		return ConnectionTypeStrings[0];
	}
}

void SZEDLiveLinkSourceEditorWidget::OnConnectionTypeChanged(TSharedPtr<FString> Value, ESelectInfo::Type SelectInfo)
{
	if (Value->Equals("Multicast"))
		ConnectionType = EZEDLiveLinkConnectionType::Multicast;
	else if (Value->Equals("Unicast"))
		ConnectionType = EZEDLiveLinkConnectionType::Unicast;
}


FReply SZEDLiveLinkSourceEditorWidget::OnCreateClicked() const
{
	FZEDLiveLinkSettings Settings;
    FIPv4Endpoint::Parse(IpAddress.Get()->GetText().ToString(), Settings.Endpoint);
	Settings.ConnectionType = ConnectionType;

	OnSourceCreated.ExecuteIfBound(Settings);

	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE