// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct KINETIXRUNTIME_API FKinetixNetworkConfiguration
{
	GENERATED_BODY()

	FKinetixNetworkConfiguration()
		: MaxWaitTimeBeforeEmoteExpiration(2.f), TargetFrameCacheCount(10),
	bSendLocalPosition(false), bSendLocalScale(false)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kinetix|Network|Configuration")
	float MaxWaitTimeBeforeEmoteExpiration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kinetix|Network|Configuration")
	int TargetFrameCacheCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kinetix|Network|Configuration")
	bool bSendLocalPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Kinetix|Network|Configuration")
	bool bSendLocalScale;
};
