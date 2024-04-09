#include "ZEDLiveLinkSettings.h"

#define LOCTEXT_NAMESPACE "ZEDLiveLinkSettings"


// static
FString FZEDLiveLinkSettings::ConnectionTypeToString( EZEDLiveLinkConnectionType ConnectionType )
{
	return (ConnectionType == EZEDLiveLinkConnectionType::Unicast)
		? "Unicast"
		: "Multicast";
}


//static
FZEDLiveLinkSettings FZEDLiveLinkSettings::FromString( const FString& In )
{
	FZEDLiveLinkSettings ParsedSettings;

	FString Address;
	FParse::Value( *In, TEXT("IpAddress="), Address);
	FIPv4Endpoint::Parse(Address, ParsedSettings.Endpoint);

	FString TempConnType;
	if ( FParse::Value( *In, TEXT("ConnectionType="), TempConnType ) )
	{
		if ( TempConnType.Equals( "Unicast", ESearchCase::IgnoreCase ) )
		{
			ParsedSettings.ConnectionType = EZEDLiveLinkConnectionType::Unicast;
		}
		else 
		{
			ParsedSettings.ConnectionType = EZEDLiveLinkConnectionType::Multicast;
		}
	}

	return ParsedSettings;
}


FString FZEDLiveLinkSettings::ToString() const
{
	return FString::Printf(
		TEXT( "IpAddress=\"%s\" "
		      "ConnectionType=\"%s\"" ),
		*Endpoint.ToString(),
		*ConnectionTypeToString( ConnectionType )
	);
}


#undef LOCTEXT_NAMESPACE
