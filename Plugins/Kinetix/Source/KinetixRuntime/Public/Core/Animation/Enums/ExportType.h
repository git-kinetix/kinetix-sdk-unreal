// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EExportType : uint8
{
	ET_NONE = 0				UMETA(DisplayName="None"),
	ET_AnimationClipLegacy	UMETA(DisplayName="Animation Clip Legacy")
};
