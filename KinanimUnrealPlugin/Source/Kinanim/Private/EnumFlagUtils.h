// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinanimTypes.h"

/**
 * 
 */
namespace EnumFlagUtils
{
	KINANIM_API bool ContainFlag(uint64 Enumerable, uint8 ToFind);

	KINANIM_API void RemoveFlagTr(ETransformDeclarationFlag& reference, uint8 toRemove);
	KINANIM_API void RemoveFlagBl(EBlendshapeDeclarationFlag& reference, uint8 toRemove);
}