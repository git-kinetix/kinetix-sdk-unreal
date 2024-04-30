// Copyright Kinetix. All Rights Reserved.

#pragma once

namespace KinetixMath4
{
		/// <summary>
	/// Make the quaternion's length to 1
	/// </summary>
	/// <param name="a"></param>
	/// <returns></returns>
	KINANIM_API FVector4f Normalize(FVector4f a);

	/// <summary>
	/// Dot product between a and b
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	KINANIM_API float Dot(FVector4f a, FVector4f b);

	/// <summary>
	/// Get a number between 0 and 1 defining how close the vectors are to each other.<br/>
	/// 0 means it's not the same, 1 means it's the same.
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	KINANIM_API float Accuracy(FVector4f a, FVector4f b);

	/// <summary>
	/// Get angle and axis of rotation
	/// </summary>
	/// <param name="q"></param>
	/// <param name="angle"></param>
	/// <param name="axis"></param>
	KINANIM_API void AngleAxis(FVector4f q, float& outAngle, FVector3f& outAxis);

	/// <summary>
	/// Check if 2 quaternions are close
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="threshold">Number between 0 and 1 describing how close the quaternion must be.<br/>0 meaning "exactly the same".</param>
	/// <returns></returns>
	KINANIM_API bool IsApproximately(FVector4f a, FVector4f b, float threshold);

	/// <summary>
	/// Check if 2 quaternions are close
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="threshold">Number between 0 and 1 describing how close the quaternion must be.<br/>0 meaning "exactly the same".</param>
	/// <returns></returns>
	KINANIM_API bool NullableIsApproximately(FVector4f a, bool aHasValue, FVector4f b, bool bHasValue, float threshold);

	//Source : http://www.codewee.com/view.php?idx=42
	/// <summary>
	/// Interpolate linearely between 2 quaternions
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t">Time between 0 and 1.<br/>0 will return <paramref name="a"/> and 1 will return <paramref name="b"/>.</param>
	/// <returns></returns>
	KINANIM_API FVector4f Lerp(FVector4f a, FVector4f b, float t);

	/// <summary>
	/// Spherical linear interpolation of a rotation
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	KINANIM_API FVector4f SLerp(FVector4f a, FVector4f b, float t);

	/// <summary>Returns the quaternion that results from multiplying two quaternions together.</summary>
	/// <param name="value1">The first quaternion.</param>
	/// <param name="value2">The second quaternion.</param>
	/// <returns>The product quaternion.</returns>
	KINANIM_API FVector4f QuatMulp(FVector4f value1, FVector4f value2);
}