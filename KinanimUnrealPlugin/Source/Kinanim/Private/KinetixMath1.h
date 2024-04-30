// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace KinetixMath1
{
	KINANIM_API bool IsApproximately(float a, float b, float threshold);
	KINANIM_API float Accuracy(float a, float b);
	KINANIM_API float SLerp(float a, float b, float t);
}
