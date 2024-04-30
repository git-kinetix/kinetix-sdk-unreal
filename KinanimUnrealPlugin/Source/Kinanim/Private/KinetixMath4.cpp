// Copyright Kinetix. All Rights Reserved.


#include "KinetixMath4.h"

#include "KinetixMath1.h"

const float EPSILON = 1e-6f;

FVector4f KinetixMath4::Normalize(FVector4f a)
{
	float dot = Dot(a, a);
	if (KinetixMath1::IsApproximately(dot, 1.f, EPSILON))
		return a;

	float len = (float)sqrtf(dot); //Magnitude
	if (len < EPSILON)
	{
		{
			return FVector4f
			{
				0.f, 0.f, 0.f, 1.f
			};
		}
	}

	return FVector4f
	{
		a.X / len,
		a.Y / len,
		a.Z / len,
		a.W / len
	};
}

float KinetixMath4::Dot(FVector4f a, FVector4f b)
{
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}

float KinetixMath4::Accuracy(FVector4f a, FVector4f b)
{
	a = Normalize(a);
	b = Normalize(b);

	return fabsf(Dot(a, b));
}

bool KinetixMath4::IsApproximately(FVector4f a, FVector4f b, float threshold)
{
	a = Normalize(a);
	b = Normalize(b);

	/**/ if (threshold > 1.f) threshold = 1.f;
	else if (threshold < 0.f) threshold = 0.f;

	return fabsf(Dot(a, b)) >= 1.f - threshold;
}

void KinetixMath4::AngleAxis(FVector4f q, float& outAngle, FVector3f& outAxis)
{
	float divide = (float)sqrtf(1.f - q.W * q.W);

	outAngle = 2.f * (float)acosf(q.W);
	outAxis.X = q.X / divide;
	outAxis.Y = q.Y / divide;
	outAxis.Z = q.Z / divide;
}

bool KinetixMath4::NullableIsApproximately(FVector4f a, bool aHasValue, FVector4f b, bool bHasValue, float threshold)
{
	//Value -> no value
	if (!bHasValue) return true;

	//No value -> value
	if (!aHasValue) return false;

	//If both have value
	return IsApproximately(a, b, threshold);
}

FVector4f KinetixMath4::Lerp(FVector4f a, FVector4f b, float t)
{
	FVector4f r = FVector4f(0.f,0.f,0.f,0.f);
	float t_ = 1.f - t;
	r.X = t_ * a.X + t * b.X;
	r.Y = t_ * a.Y + t * b.Y;
	r.Z = t_ * a.Z + t * b.Z;
	r.W = t_ * a.W + t * b.W;
	Normalize(r);
	return r;
}

FVector4f KinetixMath4::SLerp(FVector4f a, FVector4f b, float t)
{
	// Licensed to the .NET Foundation under one or more agreements.
	// The .NET Foundation licenses under the MIT license.

	float cosOmega = Dot(a, b);

	bool flip = false;

	if (cosOmega < 0.f)
	{
		flip = true;
		cosOmega = -cosOmega;
	}

	float s1, s2;

	if (cosOmega > (1.f - EPSILON))
	{
		// Too close, do straight linear interpolation.
		s1 = 1.0f - t;
		s2 = (flip) ? -t : t;
	}
	else
	{
		float omega = acosf(cosOmega);
		float invSinOmega = 1.f / sinf(omega);

		s1 = sinf((1.f - t) * omega) * invSinOmega;
		s2 =
			flip
			? -sinf(t * omega) * invSinOmega
			: sinf(t * omega) * invSinOmega;
	}

	FVector4f result = FVector4f(0.f,0.f,0.f,0.f);

	result.X = s1 * a.X + s2 * b.X;
	result.Y = s1 * a.Y + s2 * b.Y;
	result.Z = s1 * a.Z + s2 * b.Z;
	result.W = s1 * a.W + s2 * b.W;

	return result;
}

FVector4f KinetixMath4::QuatMulp(FVector4f value1, FVector4f value2)
{
	// Licensed to the .NET Foundation under one or more agreements.
	// The .NET Foundation licenses under the MIT license.

	FVector4f ans;

	float q1x = value1.X;
	float q1y = value1.Y;
	float q1z = value1.Z;
	float q1w = value1.W;

	float q2x = value2.X;
	float q2y = value2.Y;
	float q2z = value2.Z;
	float q2w = value2.W;

	// cross(av, bv)
	float cx = q1y * q2z - q1z * q2y;
	float cy = q1z * q2x - q1x * q2z;
	float cz = q1x * q2y - q1y * q2x;

	float dot = q1x * q2x + q1y * q2y + q1z * q2z;

	ans.X = q1x * q2w + q2x * q1w + cx;
	ans.Y = q1y * q2w + q2y * q1w + cy;
	ans.Z = q1z * q2w + q2z * q1w + cz;
	ans.W = q1w * q2w - dot;

	return ans;
}
