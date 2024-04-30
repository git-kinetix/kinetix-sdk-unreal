// Copyright Kinetix. All Rights Reserved.


#include "EnumFlagUtils.h"

bool EnumFlagUtils::ContainFlag(uint64 enumerable, uint8 toFind)
{
	return (enumerable & static_cast<uint64>(1) << toFind) != 0;
}

void EnumFlagUtils::RemoveFlagTr(ETransformDeclarationFlag& reference, uint8 toRemove)
{
	reference = static_cast<ETransformDeclarationFlag>(
		static_cast<uint64>(reference) & (static_cast<uint64>(ETransformDeclarationFlag::TDF_MaxValue) ^ (static_cast<uint64>(1) << toRemove)));
}

void EnumFlagUtils::RemoveFlagBl(EBlendshapeDeclarationFlag& reference, uint8 toRemove)
{
	reference = static_cast<EBlendshapeDeclarationFlag>(
		static_cast<uint64>(reference) & (static_cast<uint64>(EBlendshapeDeclarationFlag::BDF_MaxValue) ^ (static_cast<uint64>(1) << toRemove)));
}
