// Copyright Kinetix. All Rights Reserved.

#include "Core/Network/KinetixNetworkedPose.h"

#pragma region CustomHalf

const int16 CUSTOM_HALF = 1000;

// Available in NumericLimits.h
static short DoubleToCustomHalf(double InValue)
{
	InValue = FMath::Clamp(InValue, MIN_int16 / CUSTOM_HALF, MAX_int16 / CUSTOM_HALF);
	return short(InValue * CUSTOM_HALF);
}

static short FloatToCustomHalf(float InValue)
{
	InValue = FMath::Clamp(InValue, MIN_int16 / CUSTOM_HALF, MAX_int16 / CUSTOM_HALF);
	return short(InValue * CUSTOM_HALF);
}

static double CustomHalfToDouble(short InCustomHalf)
{
	const double returnValue = InCustomHalf;
	return returnValue / CUSTOM_HALF;
}

static float CustomHalfToFloat(short InCustomHalf)
{
	const float returnValue = InCustomHalf;
	return returnValue / CUSTOM_HALF;
}

#pragma endregion CustomHalf

void FBonePoseInfo::SerializeCompressedShort(FArchive& Ar, uint8 Flags)
{
	const bool bArLoading = Ar.IsLoading();

	/*
	 *	Orientation
	 */
	int16 ShortQX = 0;
	int16 ShortQY = 0;
	int16 ShortQZ = 0;
	int16 ShortQW = 0;

	/*
	 *	Location
	 */
	int16 ShortX = 0;
	int16 ShortY = 0;
	int16 ShortZ = 0;

	/*
	 *	Scale
	 */
	int16 ShortSX = 0;
	int16 ShortSY = 0;
	int16 ShortSZ = 0;

	if (!bArLoading)
	{
		if (true)
		{
			
		}
		ShortQX = DoubleToCustomHalf(LocalQuaternion.X);
		ShortQY = DoubleToCustomHalf(LocalQuaternion.Y);
		ShortQZ = DoubleToCustomHalf(LocalQuaternion.Z);
		ShortQW = DoubleToCustomHalf(LocalQuaternion.W);

		ShortX = FloatToCustomHalf(LocalPosition.X);
		ShortY = FloatToCustomHalf(LocalPosition.Y);
		ShortZ = FloatToCustomHalf(LocalPosition.Z);

		ShortSX = FloatToCustomHalf(LocalScale.X);
		ShortSY = FloatToCustomHalf(LocalScale.Y);
		ShortSZ = FloatToCustomHalf(LocalScale.Z);
	}

	Ar << ShortQX;
	Ar << ShortQY;
	Ar << ShortQZ;
	Ar << ShortQW;
	
	Ar << ShortX;
	Ar << ShortY;
	Ar << ShortZ;
	
	Ar << ShortSX;
	Ar << ShortSY;
	Ar << ShortSZ;

	if (bArLoading)
	{
		LocalQuaternion.X = CustomHalfToDouble(ShortQX);
		LocalQuaternion.Y = CustomHalfToDouble(ShortQY);
		LocalQuaternion.Z = CustomHalfToDouble(ShortQZ);
		LocalQuaternion.W = CustomHalfToDouble(ShortQW);

		LocalPosition.X = CustomHalfToDouble(ShortX);
		LocalPosition.Y = CustomHalfToDouble(ShortY);
		LocalPosition.Z = CustomHalfToDouble(ShortZ);

		LocalScale.X = CustomHalfToDouble(ShortSX);
		LocalScale.Y = CustomHalfToDouble(ShortSY);
		LocalScale.Z = CustomHalfToDouble(ShortSZ);
	}
}

bool FKinetixNetworkedPose::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Guid;
	Ar << TimeStamp;
	uint8 Flags = (bPosEnabled << 0)
		| (bScaleEnabled << 1)
		| (bHasBlendshapes << 2)
		| (bHasArmature << 3);

	Ar.SerializeBits(&Flags, 4);

	for (int i = 0; i < Bones.Num(); ++i)
	{
		Bones[i].SerializeCompressedShort(Ar, Flags);
	}

	return true;
}