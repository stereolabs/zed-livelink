#include "ZEDLiveLinkSourceFactory.h"
#include "ZEDLiveLinkSource.h"
#include "SZEDLiveLinkSourceEditorWidget.h"

#define LOCTEXT_NAMESPACE "ZEDLiveLinkSourceFactory"

FText UZEDLiveLinkSourceFactory::GetSourceDisplayName() const
{
	return LOCTEXT("SourceDisplayName", "Stereolabs Source");
}

FText UZEDLiveLinkSourceFactory::GetSourceTooltip() const
{
	return LOCTEXT("SourceTooltip", "Creates a connection to a ZED UDP Stream");
}

TSharedPtr<SWidget> UZEDLiveLinkSourceFactory::BuildCreationPanel(FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
	return SNew(SZEDLiveLinkSourceEditorWidget)
		.OnSourceCreated(FZEDLiveLinkOnSourceCreated::CreateUObject(
			this, &UZEDLiveLinkSourceFactory::OnSourceCreated, InOnLiveLinkSourceCreated
		));
}

TSharedPtr<ILiveLinkSource> UZEDLiveLinkSourceFactory::CreateSource(const FString& InConnectionString) const
{
	return MakeShared<FZEDLiveLinkSource>(FZEDLiveLinkSettings::FromString(InConnectionString));
}

void UZEDLiveLinkSourceFactory::OnSourceCreated(FZEDLiveLinkSettings InSettings, FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
	InOnLiveLinkSourceCreated.ExecuteIfBound(MakeShared<FZEDLiveLinkSource>(InSettings), InSettings.ToString());
}

#undef LOCTEXT_NAMESPACE