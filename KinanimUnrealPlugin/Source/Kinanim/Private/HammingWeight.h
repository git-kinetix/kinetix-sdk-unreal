// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace HammingWeight
{
	KINANIM_API uint8 GetHammingWeightC(int8 a);
	KINANIM_API uint8 GetHammingWeightUC(uint8 a);
	KINANIM_API uint16 GetHammingWeightS(int16 a);
	KINANIM_API uint16 GetHammingWeightUS(uint16 a);
	KINANIM_API uint32 GetHammingWeightL(int32 a);
	KINANIM_API uint32 GetHammingWeightUL(uint32 a);
	KINANIM_API uint64 GetHammingWeightLL(int64 a);
	KINANIM_API uint64 GetHammingWeightULL(uint64 a);
}