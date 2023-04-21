// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinetixDataLibrary.h"
#include "Engine/DataAsset.h"
#include "AnimationMetadataAsset.generated.h"

/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UAnimationMetadataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category="Animation", meta=(ShowOnlyInnerProperties, CustomizeProperty))
	FAnimationMetadata AnimationMetadata;
};
