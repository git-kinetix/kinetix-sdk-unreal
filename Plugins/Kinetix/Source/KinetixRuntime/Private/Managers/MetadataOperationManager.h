// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "Tasks/Task.h"

/**
 * 
 */
class FMetadataOperationManager
{
public:
	FMetadataOperationManager();
	~FMetadataOperationManager();

	static void GetAnimationMetadataOfEmote(const FAnimationMetadata& InAnimationID,
	                                        const TDelegate<void(FAnimationMetadata InAnimationMetadata)>& Callback);
};
