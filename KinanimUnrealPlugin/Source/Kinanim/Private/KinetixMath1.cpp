// Copyright Kinetix. All Rights Reserved.


#include "KinetixMath1.h"

bool KinetixMath1::IsApproximately(float a, float b, float threshold)
{
	return fabsf(a - b) <= threshold;
}

float KinetixMath1::Accuracy(float a, float b)
{
	a = fminf(fmaxf(a, -1.f), 1.f);
	b = fminf(fmaxf(b, -1.f), 1.f);

	float normalisedA = sqrtf(1.f - a * a);
	float normalisedB = sqrtf(1.f - b * b);
	
	return fmaxf(
		fabsf(a * b + normalisedA * normalisedB),
		fabsf(a * b - normalisedA * normalisedB)
	);
}

float KinetixMath1::SLerp(float a, float b, float t)
{
	float cosOmega = a * b;

	bool flip = false;

	if (cosOmega < 0.0f)
	{
		flip = true;
		cosOmega = -cosOmega;
	}

	float s1, s2;

	if (cosOmega > (1.0f - 1e-6f))
	{
		// Too close, do straight linear interpolation.
		s1 = 1.0f - t;
		s2 = (flip) ? -t : t;
	}
	else
	{
		float omega = acosf(cosOmega);
		float invSinOmega = 1.f / sinf(omega);

		s1 = sinf((1.0f - t) * omega) * invSinOmega;
		s2 = (flip)
			? -sinf(t * omega) * invSinOmega
			: sinf(t * omega) * invSinOmega;
	}

	return s1 * a + s2 * b;
}