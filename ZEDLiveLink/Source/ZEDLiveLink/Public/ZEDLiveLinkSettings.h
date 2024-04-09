#pragma once

#include <LiveLinkSourceSettings.h>
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "ZEDLiveLinkSettings.generated.h"


UENUM( BlueprintType )
enum class EZEDLiveLinkConnectionType : uint8
{
	Unicast	= 0  UMETA(DisplayName = "Unicast"),
	Multicast    UMETA(DisplayName = "Multicast")
};


struct ZEDLIVELINK_API FZEDLiveLinkSettings
{
public:
	static FString ConnectionTypeToString( EZEDLiveLinkConnectionType ConnectionType );

	static FZEDLiveLinkSettings FromString( const FString& In );

public:
	FString ToString() const;

	FIPv4Endpoint Endpoint;
	EZEDLiveLinkConnectionType ConnectionType = EZEDLiveLinkConnectionType::Unicast;
};


/** Live link streaming settings */
UCLASS()
class ZEDLIVELINK_API UZEDLiveLinkSourceSettings : public ULiveLinkSourceSettings
{
public:
	GENERATED_BODY()

	/** Origin of streamed data. */
	UPROPERTY(EditAnywhere, Category = "DataPreprocessing")
	FVector InitialLocation = FVector::ZeroVector;

	/** Orientation of streamed data. */
	UPROPERTY(EditAnywhere, Category = "DataPreprocessing")
	FRotator InitialOrientation = FRotator::ZeroRotator;
};
