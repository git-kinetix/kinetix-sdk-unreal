// Copyright Kinetix. All Rights Reserved.

#include "InterpoCompression.h"

#include "EnumFlagUtils.h"
#include "HammingWeight.h"
#include "KinetixMath4.h"
#include "KinetixMath1.h"
#include <functional>
#include <vector>
#include <cmath>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef MIN
#define MIN(a, b) ((a<b) ? a : b)
#endif

#ifndef MAX
#define MAX(a, b) ((a>b) ? a : b)
#endif

#define DEFINE_NULLABLE_ROTATION(variableName, ...)\
FVector4f variableName = __VA_ARGS__.Rotation;\
bool variableName##_hasValue = __VA_ARGS__.bHasRotation;

#define SET_NULLABLE_ROTATION(variableName, ...)\
variableName = __VA_ARGS__.Rotation;\
variableName##_hasValue = __VA_ARGS__.bHasRotation;

/// <summary>
/// Don't allow frames in interpolations to have more than 2% error
/// </summary>
const float MIN_LERP_ACCURACY = 0.98f;
/// <summary>
/// Don't allow frames in interpolations to have more than 1% error
/// </summary>
const float MIN_LERP_ACCURACY_FOOT = 0.99f;

const float InterpoCompression::EPSILON = 1E-7f;


template<class T>
struct ArrayWithLenght
{
public:
	int length;
	T* arr;

	T& operator[](int index) {
		return arr[index];
	}
};

/// <summary>
// A dictionary to get accuracy for specific transform
// </summary>
const std::map<EKinanimTransform, float> LERP_ACCURACY = 
{
	{EKinanimTransform::KT_LeftUpperLeg,  MIN_LERP_ACCURACY_FOOT},
	{EKinanimTransform::KT_LeftLowerLeg,  MIN_LERP_ACCURACY_FOOT},
	{EKinanimTransform::KT_LeftFoot,      MIN_LERP_ACCURACY_FOOT},
	{EKinanimTransform::KT_LeftToes,      MIN_LERP_ACCURACY_FOOT},
	{EKinanimTransform::KT_RightUpperLeg, MIN_LERP_ACCURACY_FOOT},
	{EKinanimTransform::KT_RightLowerLeg, MIN_LERP_ACCURACY_FOOT},
	{EKinanimTransform::KT_RightFoot,     MIN_LERP_ACCURACY_FOOT},
	{EKinanimTransform::KT_RightToes,     MIN_LERP_ACCURACY_FOOT},
};

const EKinanimTransform transformPriorityOrder[static_cast<unsigned long>(EKinanimTransform::KT_Count)] =
{
	EKinanimTransform::KT_RightLittleDistal,
	EKinanimTransform::KT_RightRingDistal,
	EKinanimTransform::KT_RightMiddleDistal,
	EKinanimTransform::KT_RightIndexDistal,
	EKinanimTransform::KT_RightThumbDistal,
	EKinanimTransform::KT_LeftLittleDistal,
	EKinanimTransform::KT_LeftRingDistal,
	EKinanimTransform::KT_LeftMiddleDistal,
	EKinanimTransform::KT_LeftIndexDistal,
	EKinanimTransform::KT_LeftThumbDistal,
	EKinanimTransform::KT_Jaw,
	EKinanimTransform::KT_RightEye,
	EKinanimTransform::KT_LeftEye,
	EKinanimTransform::KT_Head,
	EKinanimTransform::KT_RightToes,
	EKinanimTransform::KT_LeftToes,
	EKinanimTransform::KT_RightLittleIntermediate,
	EKinanimTransform::KT_RightRingIntermediate,
	EKinanimTransform::KT_RightMiddleIntermediate,
	EKinanimTransform::KT_RightIndexIntermediate,
	EKinanimTransform::KT_RightThumbIntermediate,
	EKinanimTransform::KT_LeftLittleIntermediate,
	EKinanimTransform::KT_LeftRingIntermediate,
	EKinanimTransform::KT_LeftMiddleIntermediate,
	EKinanimTransform::KT_LeftIndexIntermediate,
	EKinanimTransform::KT_LeftThumbIntermediate,
	EKinanimTransform::KT_Neck,
	EKinanimTransform::KT_RightFoot,
	EKinanimTransform::KT_LeftFoot,
	EKinanimTransform::KT_RightLittleProximal,
	EKinanimTransform::KT_RightRingProximal,
	EKinanimTransform::KT_RightMiddleProximal,
	EKinanimTransform::KT_RightIndexProximal,
	EKinanimTransform::KT_RightThumbProximal,
	EKinanimTransform::KT_LeftLittleProximal,
	EKinanimTransform::KT_LeftRingProximal,
	EKinanimTransform::KT_LeftMiddleProximal,
	EKinanimTransform::KT_LeftIndexProximal,
	EKinanimTransform::KT_LeftThumbProximal,
	EKinanimTransform::KT_RightLowerLeg,
	EKinanimTransform::KT_LeftLowerLeg,
	EKinanimTransform::KT_RightHand,
	EKinanimTransform::KT_LeftHand,
	EKinanimTransform::KT_RightUpperLeg,
	EKinanimTransform::KT_LeftUpperLeg,
	EKinanimTransform::KT_RightLowerArm,
	EKinanimTransform::KT_LeftLowerArm,
	EKinanimTransform::KT_RightUpperArm,
	EKinanimTransform::KT_LeftUpperArm,
	EKinanimTransform::KT_RightShoulder,
	EKinanimTransform::KT_LeftShoulder,
	EKinanimTransform::KT_UpperChest,
	EKinanimTransform::KT_Chest,
	EKinanimTransform::KT_Spine,
	EKinanimTransform::KT_Hips,
};

/// <summary>
/// Gives the parrent of a transform
/// </summary>
const std::map<EKinanimTransform, EKinanimTransform> transformParent =
{
	{EKinanimTransform::KT_LeftUpperLeg,             EKinanimTransform::KT_Hips},
	{EKinanimTransform::KT_RightUpperLeg,            EKinanimTransform::KT_Hips},
	{EKinanimTransform::KT_LeftLowerLeg,             EKinanimTransform::KT_LeftUpperLeg},
	{EKinanimTransform::KT_RightLowerLeg,            EKinanimTransform::KT_RightUpperLeg},
	{EKinanimTransform::KT_LeftFoot,                 EKinanimTransform::KT_LeftLowerLeg},
	{EKinanimTransform::KT_RightFoot,                EKinanimTransform::KT_RightLowerLeg},
	{EKinanimTransform::KT_LeftToes,                 EKinanimTransform::KT_LeftFoot},
	{EKinanimTransform::KT_RightToes,                EKinanimTransform::KT_RightFoot},
	{EKinanimTransform::KT_Spine,                    EKinanimTransform::KT_Hips},
	{EKinanimTransform::KT_Chest,                    EKinanimTransform::KT_Spine},
	{EKinanimTransform::KT_UpperChest,               EKinanimTransform::KT_Chest},
	{EKinanimTransform::KT_Neck,                     EKinanimTransform::KT_UpperChest},
	{EKinanimTransform::KT_Head,                     EKinanimTransform::KT_Neck},
	{EKinanimTransform::KT_LeftEye,                  EKinanimTransform::KT_Head},
	{EKinanimTransform::KT_RightEye,                 EKinanimTransform::KT_Head},
	{EKinanimTransform::KT_Jaw,                      EKinanimTransform::KT_Head},
	{EKinanimTransform::KT_LeftShoulder,             EKinanimTransform::KT_UpperChest},
	{EKinanimTransform::KT_RightShoulder,            EKinanimTransform::KT_UpperChest},
	{EKinanimTransform::KT_LeftUpperArm,             EKinanimTransform::KT_LeftShoulder},
	{EKinanimTransform::KT_RightUpperArm,            EKinanimTransform::KT_RightShoulder},
	{EKinanimTransform::KT_LeftLowerArm,             EKinanimTransform::KT_LeftUpperArm},
	{EKinanimTransform::KT_RightLowerArm,            EKinanimTransform::KT_RightUpperArm},
	{EKinanimTransform::KT_LeftHand,                 EKinanimTransform::KT_LeftLowerArm},
	{EKinanimTransform::KT_RightHand,                EKinanimTransform::KT_RightLowerArm},
	{EKinanimTransform::KT_LeftThumbProximal,        EKinanimTransform::KT_LeftHand},
	{EKinanimTransform::KT_LeftThumbIntermediate,    EKinanimTransform::KT_LeftThumbProximal},
	{EKinanimTransform::KT_LeftThumbDistal,          EKinanimTransform::KT_LeftThumbIntermediate},
	{EKinanimTransform::KT_LeftIndexProximal,        EKinanimTransform::KT_LeftHand},
	{EKinanimTransform::KT_LeftIndexIntermediate,    EKinanimTransform::KT_LeftIndexProximal},
	{EKinanimTransform::KT_LeftIndexDistal,          EKinanimTransform::KT_LeftIndexIntermediate},
	{EKinanimTransform::KT_LeftMiddleProximal,       EKinanimTransform::KT_LeftHand},
	{EKinanimTransform::KT_LeftMiddleIntermediate,   EKinanimTransform::KT_LeftMiddleProximal},
	{EKinanimTransform::KT_LeftMiddleDistal,         EKinanimTransform::KT_LeftMiddleIntermediate},
	{EKinanimTransform::KT_LeftRingProximal,         EKinanimTransform::KT_LeftHand},
	{EKinanimTransform::KT_LeftRingIntermediate,     EKinanimTransform::KT_LeftRingProximal},
	{EKinanimTransform::KT_LeftRingDistal,           EKinanimTransform::KT_LeftRingIntermediate},
	{EKinanimTransform::KT_LeftLittleProximal,       EKinanimTransform::KT_LeftHand},
	{EKinanimTransform::KT_LeftLittleIntermediate,   EKinanimTransform::KT_LeftLittleProximal},
	{EKinanimTransform::KT_LeftLittleDistal,         EKinanimTransform::KT_LeftLittleIntermediate},
	{EKinanimTransform::KT_RightThumbProximal,       EKinanimTransform::KT_RightHand},
	{EKinanimTransform::KT_RightThumbIntermediate,   EKinanimTransform::KT_RightThumbProximal},
	{EKinanimTransform::KT_RightThumbDistal,         EKinanimTransform::KT_RightThumbIntermediate},
	{EKinanimTransform::KT_RightIndexProximal,       EKinanimTransform::KT_RightHand},
	{EKinanimTransform::KT_RightIndexIntermediate,   EKinanimTransform::KT_RightIndexProximal},
	{EKinanimTransform::KT_RightIndexDistal,         EKinanimTransform::KT_RightIndexIntermediate},
	{EKinanimTransform::KT_RightMiddleProximal,      EKinanimTransform::KT_RightHand},
	{EKinanimTransform::KT_RightMiddleIntermediate,  EKinanimTransform::KT_RightMiddleProximal},
	{EKinanimTransform::KT_RightMiddleDistal,        EKinanimTransform::KT_RightMiddleIntermediate},
	{EKinanimTransform::KT_RightRingProximal,        EKinanimTransform::KT_RightHand},
	{EKinanimTransform::KT_RightRingIntermediate,    EKinanimTransform::KT_RightRingProximal},
	{EKinanimTransform::KT_RightRingDistal,          EKinanimTransform::KT_RightRingIntermediate},
	{EKinanimTransform::KT_RightLittleProximal,      EKinanimTransform::KT_RightHand},
	{EKinanimTransform::KT_RightLittleIntermediate,  EKinanimTransform::KT_RightLittleProximal},
	{EKinanimTransform::KT_RightLittleDistal,        EKinanimTransform::KT_RightLittleIntermediate},
};

/// <summary>
/// Gives the children of a transform
/// </summary>
const std::map<EKinanimTransform, ArrayWithLenght<EKinanimTransform>> transformChildren =
{
	{EKinanimTransform::KT_LeftHand,                {5, new EKinanimTransform[5]{EKinanimTransform::KT_LeftThumbProximal ,EKinanimTransform::KT_LeftIndexProximal ,EKinanimTransform::KT_LeftMiddleProximal ,EKinanimTransform::KT_LeftRingProximal ,EKinanimTransform::KT_LeftLittleProximal} }},
	{EKinanimTransform::KT_RightHand,               {5, new EKinanimTransform[5]{EKinanimTransform::KT_RightThumbProximal ,EKinanimTransform::KT_RightIndexProximal ,EKinanimTransform::KT_RightMiddleProximal ,EKinanimTransform::KT_RightRingProximal ,EKinanimTransform::KT_RightLittleProximal} }},
	{EKinanimTransform::KT_UpperChest,              {3, new EKinanimTransform[3]{EKinanimTransform::KT_Neck ,EKinanimTransform::KT_LeftShoulder ,EKinanimTransform::KT_RightShoulder} }},
	{EKinanimTransform::KT_Head,                    {3, new EKinanimTransform[3]{EKinanimTransform::KT_LeftEye ,EKinanimTransform::KT_RightEye ,EKinanimTransform::KT_Jaw} }},
	{EKinanimTransform::KT_LeftUpperLeg,            {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftLowerLeg} }},
	{EKinanimTransform::KT_RightUpperLeg,           {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightLowerLeg} }},
	{EKinanimTransform::KT_LeftLowerLeg,            {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftFoot} }},
	{EKinanimTransform::KT_RightLowerLeg,           {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightFoot} }},
	{EKinanimTransform::KT_LeftFoot,                {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftToes} }},
	{EKinanimTransform::KT_RightFoot,               {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightToes} }},
	{EKinanimTransform::KT_Spine,                   {1, new EKinanimTransform[1]{EKinanimTransform::KT_Chest} }},
	{EKinanimTransform::KT_Chest,                   {1, new EKinanimTransform[1]{EKinanimTransform::KT_UpperChest} }},
	{EKinanimTransform::KT_Neck,                    {1, new EKinanimTransform[1]{EKinanimTransform::KT_Head} }},
	{EKinanimTransform::KT_LeftShoulder,            {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftUpperArm} }},
	{EKinanimTransform::KT_RightShoulder,           {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightUpperArm} }},
	{EKinanimTransform::KT_LeftUpperArm,            {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftLowerArm} }},
	{EKinanimTransform::KT_RightUpperArm,           {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightLowerArm} }},
	{EKinanimTransform::KT_LeftLowerArm,            {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftHand} }},
	{EKinanimTransform::KT_RightLowerArm,           {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightHand} }},
	{EKinanimTransform::KT_LeftThumbProximal,       {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftThumbIntermediate} }},
	{EKinanimTransform::KT_LeftThumbIntermediate,   {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftThumbDistal} }},
	{EKinanimTransform::KT_LeftIndexProximal,       {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftIndexIntermediate} }},
	{EKinanimTransform::KT_LeftIndexIntermediate,   {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftIndexDistal} }},
	{EKinanimTransform::KT_LeftMiddleProximal,      {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftMiddleIntermediate} }},
	{EKinanimTransform::KT_LeftMiddleIntermediate,  {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftMiddleDistal} }},
	{EKinanimTransform::KT_LeftRingProximal,        {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftRingIntermediate} }},
	{EKinanimTransform::KT_LeftRingIntermediate,    {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftRingDistal} }},
	{EKinanimTransform::KT_LeftLittleProximal,      {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftLittleIntermediate} }},
	{EKinanimTransform::KT_LeftLittleIntermediate,  {1, new EKinanimTransform[1]{EKinanimTransform::KT_LeftLittleDistal} }},
	{EKinanimTransform::KT_RightThumbProximal,      {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightThumbIntermediate} }},
	{EKinanimTransform::KT_RightThumbIntermediate,  {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightThumbDistal} }},
	{EKinanimTransform::KT_RightIndexProximal,      {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightIndexIntermediate} }},
	{EKinanimTransform::KT_RightIndexIntermediate,  {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightIndexDistal} }},
	{EKinanimTransform::KT_RightMiddleProximal,     {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightMiddleIntermediate} }},
	{EKinanimTransform::KT_RightMiddleIntermediate, {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightMiddleDistal} }},
	{EKinanimTransform::KT_RightRingProximal,       {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightRingIntermediate} }},
	{EKinanimTransform::KT_RightRingIntermediate,   {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightRingDistal} }},
	{EKinanimTransform::KT_RightLittleProximal,     {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightLittleIntermediate} }},
	{EKinanimTransform::KT_RightLittleIntermediate, {1, new EKinanimTransform[1]{EKinanimTransform::KT_RightLittleDistal} }},
	{EKinanimTransform::KT_Jaw,                     {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_LeftEye,                 {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_RightEye,                {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_RightToes,               {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_LeftToes,                {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_LeftThumbDistal,         {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_LeftIndexDistal,         {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_LeftMiddleDistal,        {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_LeftRingDistal,          {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_LeftLittleDistal,        {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_RightThumbDistal,        {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_RightIndexDistal,        {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_RightMiddleDistal,       {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_RightRingDistal,         {0, new EKinanimTransform[0]{} }},
	{EKinanimTransform::KT_RightLittleDistal,       {0, new EKinanimTransform[0]{} }}
};

/// <summary>
/// Get the accuracy of each child end bone in global context
/// </summary>
ArrayWithLenght<float> GlobalAccuracyInternal(EKinanimTransform tr, FFrameData originalFrame, FFrameData originalFramePrevious, FVector4f compareValue, FVector4f compareValuePrevious, FVector4f* originalValue = nullptr, FVector4f* originalValuePrevious = nullptr)
{
	//Rotation
	bool rotation_hasValue = originalValue != nullptr;
	FVector4f rotation = rotation_hasValue ? *originalValue : originalFrame.Transforms[static_cast<uint32>(tr)].Rotation;
	//RotationPrevious
	bool rotationPrevious_hasValue = originalValuePrevious != nullptr;
	FVector4f rotationPrevious = rotation_hasValue ? *originalValuePrevious : originalFramePrevious.Transforms[static_cast<uint32>(tr)].Rotation;

	auto Linq = [tr, originalFrame, originalFramePrevious, compareValue, compareValuePrevious, rotation, rotation_hasValue, rotationPrevious, rotationPrevious_hasValue](EKinanimTransform tr2)
		{
			//Basically : "child rotation in original parent context" and then call recursion
			FVector4f newCompare = compareValue;
			DEFINE_NULLABLE_ROTATION(rotation2, originalFrame.Transforms[static_cast<uint32>(tr2)]);
			FVector4f newComparePrevious = compareValuePrevious;
			DEFINE_NULLABLE_ROTATION(rotation2Previous, originalFrame.Transforms[static_cast<uint32>(tr2)]);

			if (rotation2_hasValue)
			{
				newCompare = KinetixMath4::QuatMulp(compareValue, rotation2);

				rotation2 = KinetixMath4::QuatMulp(rotation_hasValue ? rotation : FVector4f{0, 0, 0, 1}, rotation2);

				newComparePrevious = KinetixMath4::QuatMulp(compareValuePrevious, rotation2Previous);
				rotation2Previous = KinetixMath4::QuatMulp(rotationPrevious_hasValue ? rotationPrevious : FVector4f{0, 0, 0, 1}, rotation2Previous);
			}
			else
			{
				rotation2 = rotation;
				rotation2Previous = rotationPrevious;
			}


			return GlobalAccuracyInternal(tr2, originalFrame, originalFramePrevious, newCompare, compareValuePrevious, &rotation2, &rotation2Previous);
		};

	//This is a end bone, we can do a return
	if (transformChildren.at(tr).length == 0)
	{
		if (!rotation_hasValue) //Current frames doesn't contain any value even in parents
			return {0, nullptr};
		else if (!rotationPrevious_hasValue) //Next frame doesn't contain any value even in parents
			return {1, new float[1] { KinetixMath4::Accuracy(rotation, compareValue) }};
		else //Compare the quantity of movement between the 2 frames
			return {1, new float[1] { 1 - std::abs(KinetixMath4::Accuracy(compareValuePrevious, compareValue) - KinetixMath4::Accuracy(rotation, rotationPrevious)) }};
	}

	std::vector<float> toReturnVector;
	ArrayWithLenght<EKinanimTransform> children = transformChildren.at(tr);
	for (int i = 0; i < children.length; i++)
	{
		ArrayWithLenght<float> linqResult = Linq(tr);
		for (int j = 0; j < linqResult.length; j++)
		{
			toReturnVector.push_back(linqResult[j]);
		}
	}

	ArrayWithLenght<float> toReturn = {(int)toReturnVector.size(), new float[(int)toReturnVector.size()]};
	std::memcpy(toReturn.arr, toReturnVector.data(), toReturnVector.size());
	
	return toReturn;
}

/// <summary>
/// Get the accuracy of each child end bone in global context and average them
/// </summary>
float GlobalAccuracy(EKinanimTransform tr, FFrameData originalFrame, FFrameData originalFramePrevious, FVector4f compareValue, FVector4f compareValuePrevious)
{

	ArrayWithLenght<float> floats = GlobalAccuracyInternal(tr, originalFrame, originalFramePrevious, compareValue, compareValuePrevious);
	if (floats.length == 0)
		return 1;

	float sum = 0;
	for (int i = floats.length - 1; i >= 0; i--)
	{
		sum += floats[i];
	}
	return sum / floats.length;
}

InterpoCompression::InterpoCompression(unsigned short maxFramePerLerp, float threshold, float blendshapeThrshold)
{
	this->maxFramePerLerp = maxFramePerLerp;
	this->threshold = threshold;
	this->blendshapeThrshold = blendshapeThrshold;
}

InterpoCompression::~InterpoCompression()
{
}

int32 InterpoCompression::GetMaxUncompressedFrame()
{
	return target->Header->hasBlendshapes ? 
		MIN(GetMaxUncompressedTransforms(), GetMaxUncompressedBlendshapes()) :
		GetMaxUncompressedTransforms();
}

int32 InterpoCompression::GetMaxUncompressedTransforms()
{
	return _maxUncompressedTransforms;
}

int32 InterpoCompression::GetMaxUncompressedBlendshapes()
{
	return _maxUncompressedBlendshapes;
}

void InterpoCompression::InitTarget(FKinanimData* data)
{
	target = data;
}

/// <summary>
/// Get a threshold for quaternion threshold calculations based on average movement quantity
/// </summary>
/// <returns>
/// Returns a lower number if the average movement quantity is higher.
/// </returns>
float InterpoCompression::CalculateThreshold(float movementQuantity, int frameCount)
{
	if (movementQuantity < EPSILON)
	{
		return 1.f;
	}

	return (1.f + (float)std::sinf((float)M_PI * (movementQuantity / frameCount + 0.5f))) / 2.f;
}

/// <summary>
/// Get global movement from <see cref="V4Utils.Accuracy(Vector4F, Vector4F)"/>
/// </summary>
/// <param name="tr">Transform to get its movement</param>
/// <param name="frames">List of frames</param>
/// <param name="from">Start frame index</param>
/// <param name="to">End frame index</param>
/// <returns>
/// Returns the sum of the quantity of movement from a frame to another<br/>
/// </returns>
/// <remarks>
/// <b>Remark:</b>
/// Being based on a dot product, the maximum possible value is <code><see langword="int"/> max = <paramref name="to"/> - <paramref name="from"/> + 1</code> 
/// </remarks>
/// <exception cref="ArgumentNullException"></exception>
float InterpoCompression::CalculateMovement(EKinanimTransform tr, FFrameData frames[], int from, int to)
{
	//Get Global rot
	auto GetGlobalRot = [tr,frames,from,to](int f, FVector4f& toReturn)
	{
		FVector4f frame = frames[f].Transforms[static_cast<uint32>(tr)].Rotation;
		bool frame_HasValue = frames[f].Transforms[static_cast<uint32>(tr)].bHasRotation;
		if (!frame_HasValue) //Ignore if doesn't have a frame
			return false;

		toReturn = frame;

		EKinanimTransform parent = tr;
		auto search = transformParent.find(parent);
		while (search != transformParent.end()) //iterate on each parrent
		{
			parent = search->second;
			frame = frames[f].Transforms[static_cast<uint32>(parent)].Rotation;
			frame_HasValue = frames[f].Transforms[static_cast<uint32>(parent)].bHasRotation;
			if (frame_HasValue)
			{
				toReturn = KinetixMath4::QuatMulp(frame, toReturn); //put toReturn in the parrent's context
			}

			search = transformParent.find(tr);
		}

		return true;
	};

	float sum = 0;
	FVector4f rotationPrev;
	bool rotationPrev_hasValue = GetGlobalRot(from, rotationPrev); //Previous glob rot
	for (int i = from + 1; i <= to; i++)
	{
		FVector4f rotation;
		bool rotation_HasValue = GetGlobalRot(i, rotation); //current glob rot
		if (rotationPrev_hasValue && rotation_HasValue)
			sum += 1 - KinetixMath4::Accuracy(rotationPrev, rotation); //prev dot current
	}

	return sum;
}

void InterpoCompression::Compress()
{
	TArray<EKeyType> keyTypes = target->Header->KeyTypes;
	FFrameData* frames = target->Content->frames;
	int frameCount = target->Header->GetFrameCount();

	int compressableTransform = 0;
	for (int i = 0; i < static_cast<uint32>(EKinanimTransform::KT_Count); i++)
	{
#pragma warning( push )
#pragma warning( disable: 26813)

		if (keyTypes[i] == EKeyType::KT_Rotation)
			++compressableTransform;

#pragma warning( pop )
	}
	compressableTransform *= frameCount;

	int compressableBlendshape = 0;
	for (int i = 0; i < frameCount; i++)
	{
		compressableBlendshape += HammingWeight::GetHammingWeightULL(static_cast<uint64>(frames[0].BlendshapeDeclarationFlag));
	}
	
	unsigned short numberOfFrameCut = 0;
	if (threshold != 0)
		CompressTransforms(numberOfFrameCut, frameCount);
	unsigned short transformOnlyCut = numberOfFrameCut;
	if (blendshapeThrshold != 0)
		CompressBlendshapes(numberOfFrameCut, frameCount);

	int compressableFrame = compressableTransform + compressableBlendshape;
}

void InterpoCompression::CompressTransforms(unsigned short& numberOfFrameCut, int frameCount)
{
	FKinanimContent* uncompressed = target->Content->Clone();

	FKinanimHeader*  header = target->Header;
	FKinanimContent* content = target->Content;

	for (uint8 i = 0; i < static_cast<uint8>(EKinanimTransform::KT_Count); i++)
	{
		EKinanimTransform tr = transformPriorityOrder[i];

		if (header->KeyTypes[static_cast<uint32>(tr)] != EKeyType::KT_Rotation)
			continue;

		float minAccuracy;
		auto search = LERP_ACCURACY.find(tr);
		if (search != LERP_ACCURACY.end())
		{
			//default
			minAccuracy = MIN_LERP_ACCURACY;
		}
		else
			minAccuracy = search->second;

		float movementQuantity = CalculateMovement(tr, uncompressed->frames, 0, frameCount - 1);
		float movementThreshold = CalculateThreshold(movementQuantity, frameCount);
		minAccuracy *= movementThreshold;
		movementThreshold = (1 - movementThreshold) * this->threshold;

		FKinanimContent* targetContent = target->Content;

		std::vector<int> interpolableFrames;
		for (int iStart = 0; iStart < frameCount; iStart++)
		{

			FVector4f startV4 = targetContent->frames[iStart].Transforms[static_cast<uint32>(tr)].Rotation;
			if (!EnumFlagUtils::ContainFlag(static_cast<uint64>(targetContent->frames[iStart].TransformDeclarationFlag), static_cast<uint8>(tr)))
				continue;

			int endFrame = MIN(maxFramePerLerp + iStart, frameCount - 1);
			int countFrame = endFrame - iStart - 1; //count frames between the 2 frames
			if (countFrame <= 0)
				break;

			interpolableFrames.clear();
			for (int j = (countFrame) - 1; j >= 0; j--)
			{
				int iEnd = iStart + 2 + j; //iStart+2 because a lerp from frame 0 to frame 1 is useless
				FVector4f endV4 = targetContent->frames[iEnd].Transforms[static_cast<uint32>(tr)].Rotation;
				if (!EnumFlagUtils::ContainFlag(
					static_cast<uint64>(targetContent->frames[iEnd].TransformDeclarationFlag),
					static_cast<uint8>(tr)))
				{
					interpolableFrames.insert(interpolableFrames.begin(), iEnd);
					continue;
				}


				FVector4f currentLerpR;
				bool currentLerpR_HasValue;
				bool lastFrameHaveNoValue = true;
				float accuracyAverage = .0f;
				float currentAccuracy;

				for (int lerpI = iEnd - 1; lerpI > iStart; lerpI--)
				{
					//Check if lerpable
					currentLerpR          = targetContent->frames[lerpI].Transforms[static_cast<uint32>(tr)].Rotation;
					currentLerpR_HasValue = targetContent->frames[lerpI].Transforms[static_cast<uint32>(tr)].bHasRotation;
					if (!currentLerpR_HasValue)
					{
						if (lastFrameHaveNoValue)
							break;

						continue;
					}

					lastFrameHaveNoValue = false;

					//Compare the difference of the distances from "startV4"
					// It's like |(uncompressed[i] - start) - (lerp[i] - start)| 
					currentAccuracy = GlobalAccuracy(tr, uncompressed->frames[lerpI], uncompressed->frames[iStart],
						KinetixMath4::SLerp(
							startV4,
							endV4,
							(float)(lerpI - iStart) / (iEnd - iStart)
						),
						startV4
					);

					if (currentAccuracy < minAccuracy) //Don't allow an accuracy to be too low
					{
						accuracyAverage = -100000;
						break;
					}
					else
					{
						accuracyAverage += currentAccuracy;
					}
				}

				accuracyAverage /= (iEnd - iStart - 1);

				if (lastFrameHaveNoValue || !(accuracyAverage >= 1 - movementThreshold)) //If interpolation isn't aprox the same, we can't compress with these iEnd / iStart params.
				{
				}
				else
				{
					interpolableFrames.insert(interpolableFrames.begin(), iEnd);
				}
			}

			if (interpolableFrames.size() > 0)
			{
				//Get the highest frame possible
				int highestFrame = interpolableFrames[interpolableFrames.size() - 1];

				for (int frame = iStart + 1; frame < highestFrame; frame++)
				{
					EnumFlagUtils::RemoveFlagTr(target->Content->frames[frame].TransformDeclarationFlag, static_cast<uint8>(tr));
					target->Content->frames[frame].Transforms[static_cast<uint32>(tr)].bHasRotation = false;
					++numberOfFrameCut;
				}

				//Resume at 'iEnd' 
				iStart = highestFrame;
			}
		}
	}
}

void InterpoCompression::CompressBlendshapes(unsigned short& numberOfFrameCut, int frameCount)
{
	for (uint8 iBl = 0; iBl < static_cast<uint8>(EKinanimBlendshape::KB_Count); iBl++)
	{
		std::vector<int> interpolableFrames;
		for (int iStart = 0; iStart < frameCount; iStart++)
		{
			float startF = target->Content->frames[iStart].Blendshapes[iBl];
			if (!EnumFlagUtils::ContainFlag(
				static_cast<uint64>(target->Content->frames[iStart].BlendshapeDeclarationFlag), iBl))
				continue;

			int endFrame = MIN(maxFramePerLerp + iStart, frameCount - 1);
			int countFrame = endFrame - iStart - 1;
			if (countFrame <= 0)
				break;

			interpolableFrames.clear();
			for (int j = (countFrame)-1; j >= 0; j--)
			{
				int iEnd = iStart + 2 + j; //iStart+2 because a lerp from frame 0 to frame 1 is useless
				float endF = target->Content->frames[iEnd].Blendshapes[iBl];
				if (!EnumFlagUtils::ContainFlag(
					static_cast<uint64>(target->Content->frames[iEnd].BlendshapeDeclarationFlag), iBl)) 
				{
					interpolableFrames.insert(interpolableFrames.begin(), iEnd);
					continue;
				}

				float currentLerpF;
				bool lastFrameHaveNoValue = true;
				float accuracyAverage = .0f;
				float currentAccuracy = 0;

				int count = 0;
				for (int lerpI = iEnd - 1; lerpI > iStart; lerpI--)
				{
					++count;
					//Check if lerpable
					currentLerpF = target->Content->frames[lerpI].Blendshapes[iBl];
					if (!EnumFlagUtils::ContainFlag(
						static_cast<uint64>(target->Content->frames[lerpI].BlendshapeDeclarationFlag), iBl))
					{
						if (lastFrameHaveNoValue)
							break;

						continue;
					}

					lastFrameHaveNoValue = false;

					currentAccuracy = KinetixMath1::Accuracy(
						currentLerpF,
						KinetixMath1::SLerp(
							startF,
							endF,
							(float)(lerpI - iStart) / (iEnd - iStart)
						)
					);

					if (currentAccuracy < MIN_LERP_ACCURACY) //Don't allow an accuracy to be too low
					{
						accuracyAverage = -100000;
						break;
					}
					else
					{
						accuracyAverage += currentAccuracy;
					}
				}

				accuracyAverage /= (iEnd - iStart - 1);

				if (lastFrameHaveNoValue || !(accuracyAverage >= 1 - blendshapeThrshold)) //If interpolation isn't aprox the same, we can't compress with these iEnd / iStart params.
				{
				}
				else
				{
					interpolableFrames.insert(interpolableFrames.begin(), iEnd);
				}
			}

			if (interpolableFrames.size() > 0)
			{
				//Get the highest frame possible
				int highestFrame = interpolableFrames[interpolableFrames.size() - 1];

				for (int frame = iStart + 1; frame < highestFrame; frame++)
				{
					EnumFlagUtils::RemoveFlagBl(target->Content->frames[frame].BlendshapeDeclarationFlag, iBl);
					++numberOfFrameCut;
				}

				//Resume at 'iEnd' 
				iStart = highestFrame;
			}
		}
	}
}

void InterpoCompression::DecompressFrame(unsigned short InLoadedFrameCount)
{
	this->loadedFrameCount += InLoadedFrameCount;
	
	DecompressTransforms();
	DecompressBlendshapes();

	unsigned short lastFrameId = target->Header->GetFrameCount();
	if (this->loadedFrameCount >= lastFrameId) //When all frame loaded
	{
		for (int i = 0; i < static_cast<int32>(EKinanimTransform::KT_Count); i++)
		{
			lastNonCompressedFrame[i] = lastFrameId - 1;
		}
		for (int i = 0; i < static_cast<int32>(EKinanimBlendshape::KB_Count); i++)
		{
			lastNonCompressedBlendshape[i] = lastFrameId - 1;
		}
	}

	CalculateMaxUncompressedProperties();
}

void InterpoCompression::DecompressTransforms()
{
	for (uint8 trIndex = 0; trIndex < static_cast<uint8>(EKinanimTransform::KT_Count); trIndex++)
	{
		//We want transforms with only rotation curves
		if (target->Header->KeyTypes[trIndex] != EKeyType::KT_Rotation)
			continue;

		unsigned short current = lastNonCompressedFrame[trIndex];// "the last time we decompressed this transform we were at frame ..."
		// Check all next declared transform

		if (current >= loadedFrameCount)
			continue;

		//Sometimes, beginning frames are empty
		if (!EnumFlagUtils::ContainFlag(
			static_cast<uint64>(target->Content->frames[current].TransformDeclarationFlag), trIndex))
		{
			while (++current < loadedFrameCount)
			{
				if (EnumFlagUtils::ContainFlag(
					static_cast<uint64>(target->Content->frames[current].TransformDeclarationFlag), trIndex))
					break;
			}

			if (current >= loadedFrameCount)
				current = loadedFrameCount - 1;

			//Skip transform if 'current' is still not a frame
			if (!EnumFlagUtils::ContainFlag(
				static_cast<uint64>(target->Content->frames[current].TransformDeclarationFlag), trIndex))
			{
				//if it's been like that for more than 'maxFramePerLerp' time it means there's no more frames to decompress
				if (lastNonCompressedFrame[trIndex] + maxFramePerLerp < loadedFrameCount - 1) //-1 for security
					lastNonCompressedFrame[trIndex] = target->Header->GetFrameCount() - 1;
				continue;
			}
		}

		//Check all next declared transform
		std::vector<unsigned short> frames;
		for (unsigned short frame = current; frame < loadedFrameCount; frame++)
		{
			if (target->Content->frames[frame].Transforms[trIndex].bHasRotation)
				frames.push_back(frame);
		}

		if (frames.size() < 2)
			continue;

		//We found 2 defined values for the interpolation
		unsigned short start = frames[0];
		unsigned short end = frames[1];
		FVector4f v4Start = target->Content->frames[start].Transforms[trIndex].Rotation;
		FVector4f v4End = target->Content->frames[end].Transforms[trIndex].Rotation;
		bool sameValue = KinetixMath4::IsApproximately(v4Start, v4End, EPSILON);
		frames.erase(frames.begin() + 0); //This is to avoid getting caught in the 'if' of the forloop without lerping
		for (unsigned short frame = start + 1u; frame < loadedFrameCount; frame++)
		{
			if (frame == frames[0])
			{
				//We arrived on the "end" frame that means our lerping ends here
				frames.erase(frames.begin() + 0);

				//Check if there are no more lerping to do
				if (frames.size() == 0)
				{
					lastNonCompressedFrame[trIndex] = frame;
					break;
				}

				//There are more lerping to do so this frame become 'start' and next in 'frames' become 'end'
				start = end;
				end = frames[0];

				//Set references to avoid repoking the array everytime
				v4Start = v4End;
				v4End = target->Content->frames[end].Transforms[trIndex].Rotation;

				sameValue = KinetixMath4::IsApproximately(v4Start, v4End, EPSILON);

				continue;
			}

			//Tell the kinanim that this frame now exist and execute the SLerp from 'start' to 'end'
			target->Content->frames[frame].TransformDeclarationFlag |= static_cast<ETransformDeclarationFlag>(1ull << trIndex);

			if (!sameValue)
			{

				target->Content->frames[frame].Transforms[trIndex] = FTransformData
				{
					{0.f,0.f,0.f},
					KinetixMath4::SLerp(v4Start, v4End, static_cast<float>(frame - start) / static_cast<float>(end - start)),
					{0.f,0.f,0.f},
					false,
					true,
					false
				};
			}
			else
			{
				target->Content->frames[frame].Transforms[trIndex] = FTransformData
				{
					{0.f,0.f,0.f},
					v4Start,
					{0.f,0.f,0.f},

					false,
					true,
					false
				};
			}
		}
	}
}
void InterpoCompression::DecompressBlendshapes()
{
	for (uint8 trIndex = 0; trIndex < static_cast<uint8>(EKinanimBlendshape::KB_Count); trIndex++)
	{
		unsigned short current = lastNonCompressedBlendshape[trIndex];// "the last time we decompressed this blendshape we were at frame ..."

		if (current > loadedFrameCount)
			continue;

		//Usually, beginning frames are empty
		if (!EnumFlagUtils::ContainFlag(
			static_cast<uint64>(target->Content->frames[current].BlendshapeDeclarationFlag), trIndex))
		{
			while (++current < loadedFrameCount)
			{
				if (EnumFlagUtils::ContainFlag(
					static_cast<uint32>(target->Content->frames[current].BlendshapeDeclarationFlag), trIndex))
					break;
			}

			if (current >= loadedFrameCount)
				current = loadedFrameCount - 1;

			//Skip transform if 'current' is still not a frame
			if (!EnumFlagUtils::ContainFlag(
				static_cast<uint64>(target->Content->frames[current].BlendshapeDeclarationFlag), trIndex))
			{
				//if it's been like that for more than 'maxFramePerLerp' time it means there's no more frames to decompress
				if (lastNonCompressedBlendshape[trIndex] + maxFramePerLerp < loadedFrameCount - 1) //-1 for security
					lastNonCompressedBlendshape[trIndex] = target->Header->GetFrameCount() - 1;
				continue;
			}
		}

		//Check all next declared blendshapes
		std::vector<unsigned short> frames;
		for (unsigned short frame = current; frame < loadedFrameCount; frame++)
		{
			if (EnumFlagUtils::ContainFlag(
				static_cast<uint32>(target->Content->frames[frame].BlendshapeDeclarationFlag), trIndex))
				frames.push_back(frame);
		}
		if (frames.size() < 2)
			continue;

		//We found 2 defined values for the interpolation
		unsigned short start = frames[0];
		unsigned short end = frames[1];
		float floatStart = target->Content->frames[start].Blendshapes[trIndex];
		float floatEnd   = target->Content->frames[end].Blendshapes[trIndex];
		bool sameValue = KinetixMath1::IsApproximately(floatStart, floatEnd, EPSILON);
		frames.erase(frames.begin() + 0); //This is to avoid getting caught in the 'if' of the forloop without lerping
		for (unsigned short frame = start + 1; frame < loadedFrameCount; frame++)
		{
			if (frame == frames[0])
			{
				//We arrived on the "end" frame that means our lerping ends here
				frames.erase(frames.begin() + 0);

				//Check if there are no more lerping to do
				if (frames.size() == 0)
				{
					lastNonCompressedFrame[trIndex] = frame;
					break;
				}

				//There are more lerping to do so this frame become 'start' and next in 'frames' become 'end'
				start = end;
				end = frames[0];

				//Tell the kinanim that this frame now exist and execute the SLerp from 'start' to 'end'
				floatStart = floatEnd;
				floatEnd = target->Content->frames[end].Blendshapes[trIndex];

				sameValue = KinetixMath1::IsApproximately(floatStart, floatEnd, EPSILON);
				continue;
			}

			//Tell the kinanim that this frame now exist and execute the SLerp from 'start' to 'end'
			target->Content->frames[frame].BlendshapeDeclarationFlag |= static_cast<EBlendshapeDeclarationFlag>(1ull << trIndex);

			if (!sameValue)
			{
				target->Content->frames[frame].Blendshapes[trIndex] = KinetixMath1::SLerp(floatStart, floatEnd, static_cast<float>(frame - start) / static_cast<float>(end - start));
			}
			else
			{
				target->Content->frames[frame].Blendshapes[trIndex] = floatStart;
			}
		}
	}
}

void InterpoCompression::CalculateMaxUncompressedProperties()
{
	//----------------//
	// Transform      //
	//----------------//
	unsigned short current;
	unsigned short minV = short(65535ul); //default: Max value

	uint8 length = static_cast<uint8>(EKinanimTransform::KT_Count);
	for (unsigned char i = 0; i < length; i++)
	{
		current = lastNonCompressedFrame[i];
		if (current == 0)
			continue;

		minV = MIN(current, minV);
	}

	if (length != 0)
		_maxUncompressedTransforms = static_cast<long>(minV);
	
	//----------------//
	// Blendshape     //
	//----------------//
	minV = short(65535ul); //default: Max value

	length = static_cast<uint8>(EKinanimBlendshape::KB_Count);
	for (unsigned char i = 0; i < length; i++)
	{
		current = lastNonCompressedBlendshape[i];
		if (current == 0)
			continue;

		minV = MIN(current, minV);
	}

	if (length != 0)
		_maxUncompressedBlendshapes = static_cast<long>(minV);
}