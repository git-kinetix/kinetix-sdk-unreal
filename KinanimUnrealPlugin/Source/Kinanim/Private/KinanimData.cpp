// Copyright Kinetix. All Rights Reserved.

#include "KinanimData.h"

#include "CustomHalf.h"
#include "HammingWeight.h"
#include "KinanimPointerCollector.h"

//-----------------------------//
//        KinanimHeader        //
//-----------------------------//

unsigned short FKinanimHeader::GetFrameCount() const { return frameCount; }

void FKinanimHeader::SetFrameCount(uint16 count)
{
	if (frameSizes == nullptr)
	{
		frameCount = count;
		frameSizes = new uint16[frameCount = count];
		return;
	}

	//Short resume: Resize pointer array

	//Store temp
	uint16 oldCount = frameCount;
	uint16* temp = frameSizes;

	//Override
	frameSizes = new uint16[frameCount = count];

	uint16 min = oldCount;
	if (min > frameCount)
		min = frameCount;

	//Copy to new array
	for (uint16 i = 0; i < min; i++)
	{
		frameSizes[i] = temp[i];
	}

	//Delete temp variables
	delete[] temp;
}

FKinanimHeader::FKinanimHeader()
{
	frameCount = 0;
	version = 0;
	frameRate = 0.f;
	frameSizes = nullptr;
	binarySize = 0l;
	hasBlendshapes = false;

	KeyTypes.SetNumZeroed(static_cast<uint64>(EKinanimTransform::KT_Count));

	for (uint8 i = 0; i < static_cast<uint8>(EKinanimTransform::KT_Count); i++)
	{
		KeyTypes[i] = EKeyType::KT_None;
	}
}

FKinanimHeader::~FKinanimHeader()
{
	delete[] frameSizes;
}

FKinanimHeader* FKinanimHeader::Clone() const
{
	FKinanimHeader* toReturn = new FKinanimHeader();
	KinanimPointerCollector::CollectPointer(toReturn);
	toReturn->version = version;
	toReturn->binarySize = binarySize;
	toReturn->frameCount = frameCount;
	toReturn->frameRate = frameRate;
	toReturn->hasBlendshapes = hasBlendshapes;

	toReturn->frameSizes = new uint16[frameCount];
	for (uint16 i = 0; i < frameCount; i++) //Copy frameSizes array
	{
		toReturn->frameSizes[i] = frameSizes[i];
	}

	toReturn->KeyTypes.SetNumZeroed(static_cast<uint64>(EKinanimTransform::KT_Count));
	for (uint8 i = 0; i < static_cast<uint8>(EKinanimTransform::KT_Count); i++) //Copy keyTypes array
	{
		toReturn->KeyTypes[i] = KeyTypes[i];
	}

	return toReturn;
}

//------------------------------//
//        KinanimContent        //
//------------------------------//

FKinanimContent::FKinanimContent()
{
	_frameCount = 0;
	frames = nullptr;
}

FKinanimContent::~FKinanimContent()
{
	delete[] frames;
}

uint32 FKinanimContent::GetFrameCount() const
{
	return _frameCount;
}

void FKinanimContent::InitFrameCount(uint16 frameCount)
{
	_frameCount = frameCount;
	frames = new FFrameData[frameCount];
}

FKinanimContent* FKinanimContent::Clone() const
{
	FKinanimContent* toReturn = new FKinanimContent();
	KinanimPointerCollector::CollectPointer(toReturn);
	toReturn->InitFrameCount(_frameCount);

	for (int i = 0; i < _frameCount; i++)
	{
		toReturn->frames[i] = FrameDataUtils::Clone(frames[i]);
	}

	return toReturn;
}

//---------------------------//
//        KinanimData        //
//---------------------------//

const char* FKinanimData::FILE_EXTENSION = ".kinanim";

FKinanimData::FKinanimData()
{
	this->Header = new FKinanimHeader();
	this->Content = new FKinanimContent();
	KinanimPointerCollector::CollectPointer(Header);
	KinanimPointerCollector::CollectPointer(Content);
}

FKinanimData::FKinanimData(FKinanimHeader* InHeader, FKinanimContent* InContent)
{
	this->Header = InHeader;
	this->Content = InContent;
	KinanimPointerCollector::CollectPointer(Header); //Still collect just in case
	KinanimPointerCollector::CollectPointer(Content);//Still collect just in case
}

FKinanimData::~FKinanimData()
{
	if (Header != nullptr && KinanimPointerCollector::ReleasePointer(Header))
	{
		delete Header;
	}

	if (Content != nullptr && KinanimPointerCollector::ReleasePointer(Content))
	{
		delete Content;
	}
}

void FKinanimData::CalculateEveryFrameSize() const
{
	const uint16 count = Header->GetFrameCount();
	for (int32 i = 0; i < count; i++)
	{
		Header->frameSizes[i] = CalculateFrameSize(Content->frames[i]);
	}
}

uint16 FKinanimData::CalculateFrameSize(FFrameData data) const
{
	if (static_cast<uint64>(data.TransformDeclarationFlag) == 0
		&& static_cast<uint64>(data.BlendshapeDeclarationFlag) == 0)
		return 0;

	uint16 toReturn =
		sizeof(uint16) + //Frame ID
		sizeof(uint64); //Declaration flag
	if (static_cast<uint64>(data.TransformDeclarationFlag) != 0)
	{
		for (uint8 i = 0; i < static_cast<uint8>(EKinanimTransform::KT_Count); i++)
		{
			if ((static_cast<uint64>(data.TransformDeclarationFlag) & (static_cast<uint64>(1) << i)) == 0)
				continue;

			if ((static_cast<uint8>(Header->KeyTypes[i]) & static_cast<uint8>(EKeyType::KT_Position)) != 0)
				toReturn += CustomHalf::SIZEOF * 3;

			if ((static_cast<uint8>(Header->KeyTypes[i]) & static_cast<uint8>(EKeyType::KT_Scale)) != 0)
				toReturn += CustomHalf::SIZEOF * 3;

			if ((static_cast<uint8>(Header->KeyTypes[i]) & static_cast<uint8>(EKeyType::KT_Rotation)) != 0)
				toReturn += CustomHalf::SIZEOF * 4;
		}
	}

	if (!Header->hasBlendshapes)
		return toReturn;

	//Blendshape
	toReturn += sizeof(uint64); //Declaration flag
	toReturn += (uint16)(HammingWeight::GetHammingWeightULL(static_cast<uint64>(data.BlendshapeDeclarationFlag)) *
		CustomHalf::SIZEOF);

	return toReturn;
}


FKinanimData* FKinanimData::Clone() const
{
	FKinanimData* data = new FKinanimData(Header->Clone(), Content->Clone());
	KinanimPointerCollector::CollectPointer(data); //Still collect just in case

	return data;
}