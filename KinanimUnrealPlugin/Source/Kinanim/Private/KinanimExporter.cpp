// Copyright Kinetix. All Rights Reserved.


#include "KinanimExporter.h"

#include "KinanimPointerCollector.h"
#include "KinanimExporter.h"
#include "CustomHalf.h"
#include "KinanimID.h"
#include "EnumFlagUtils.h"
#include <stdexcept>

#define WRITE_STREAM(stream, variable) stream->write(reinterpret_cast<const char*>(&variable), sizeof(variable))

void KinanimExporter::WriteFile(std::ostream* stream, FKinanimData* data, IKinanimCompression* compression, bool clone)
{
	if (data == nullptr)
	{
		throw std::invalid_argument("data: Null pointer exception");
	}

	if (compression != nullptr)
	{
		if (clone)
		{
			data = data->Clone();
			KinanimPointerCollector::CollectPointer((IDisposable*)data);
		}


		compression->InitTarget(data);
		compression->Compress();

		data->CalculateEveryFrameSize();
	}

	WriteHeader(stream, data->Header);
	WriteContent(stream, data->Content, data->Header);

	if (clone && KinanimPointerCollector::ReleasePointer((IDisposable*)data))
	{
		delete data;
	}
}

void KinanimExporter::WriteFileHeaderContent(std::ostream* stream, FKinanimHeader* header, FKinanimContent* content)
{
	if (!stream)
	{
		throw std::invalid_argument("stream: Null stream exception");
	}

	if (header == nullptr)
	{
		throw std::invalid_argument("header: Null pointer exception");
	}

	if (content == nullptr)
	{
		throw std::invalid_argument("content: Null pointer exception");
	}

	WriteHeader(stream, header);
	WriteContent(stream, content, header);
}

void KinanimExporter::WriteHeader(std::ostream* stream, FKinanimHeader* header)
{
	Header::WriteVersion           (stream, header->version);
	Header::WriteFrameRate         (stream, header->frameRate);
	Header::WriteFrameCountAndSizes(stream, header->GetFrameCount(), header->frameSizes);
	Header::WriteKeyTypes          (stream, header->KeyTypes.GetData());
	Header::WriteHasBlendshape     (stream, header->hasBlendshapes);
	Header::WriteEndHeader         (stream);
}

void KinanimExporter::OverrideHeader(std::ostream* stream, FKinanimHeader* header, unsigned long long fileStartPositionUll)
{
	std::streampos fileStartPosition = fileStartPositionUll;

	if (!stream)
	{
		throw std::invalid_argument("stream: Null stream exception");
	}

	if (header == nullptr)
	{
		throw std::invalid_argument("header: Null pointer exception");
	}

	std::streampos position = stream->tellp();
	if (stream->seekp(fileStartPosition).fail())
	{
		throw std::out_of_range("Couldn't seekp on the stream: given position is out of range");
	}

	WriteHeader(stream, header); //The header is always of the same size so there's no need to compute header size
	
	if (stream->seekp(position).fail())
	{
		throw std::out_of_range("Couldn't seekp on the stream: given position is out of range");
	}
}

void KinanimExporter::WriteContent(std::ostream* stream, FKinanimContent* content, FKinanimHeader* referenceHeader)
{
	Content::WriteFrames(stream, content->frames, referenceHeader->GetFrameCount(), referenceHeader->hasBlendshapes);
}

// -------------------------------- //
//              HEADER              //
// -------------------------------- //

void KinanimExporter::Header::WriteVersion(std::ostream* stream, unsigned short version)
{
	WRITE_STREAM(stream, KinanimID::FIELD_ID_VERSION);
	WRITE_STREAM(stream, version);
}

void KinanimExporter::Header::WriteFrameRate(std::ostream* stream, float frameRate)
{
	WRITE_STREAM(stream, KinanimID::FIELD_ID_FRAMERATE);
	WRITE_STREAM(stream, frameRate);
}

void KinanimExporter::Header::WriteFrameCountAndSizes(std::ostream* stream, unsigned short frameCount, unsigned short* frameSizes)
{
	WRITE_STREAM(stream, KinanimID::FIELD_ID_FRAMECOUNT);
	WRITE_STREAM(stream, frameCount);
	for (int i = 0; i < frameCount; i++)
	{
		WRITE_STREAM(stream, frameSizes[i]);
	}
}

void KinanimExporter::Header::WriteKeyTypes(std::ostream* stream, EKeyType keyTypes[static_cast<unsigned long>(EKinanimTransform::KT_Count)])
{
	EKinanimTransform count = EKinanimTransform::KT_Count;
	WRITE_STREAM(stream, KinanimID::FIELD_ID_KEYTYPES);
	WRITE_STREAM(stream, count);
	for (uint8 i = 0; i < static_cast<uint8>(count); i++)
	{
		WRITE_STREAM(stream, keyTypes[i]);
	}
}

void KinanimExporter::Header::WriteHasBlendshape(std::ostream* stream, bool hasBlendshape)
{
	WRITE_STREAM(stream, KinanimID::FIELD_ID_HASBLENDSHAPES);
	WRITE_STREAM(stream, hasBlendshape);
}

void KinanimExporter::Header::WriteEndHeader(std::ostream* stream)
{
	WRITE_STREAM(stream, KinanimID::FIELD_ID_ENDHEADER);
}

// --------------------------------- //
//              CONTENT              //
// --------------------------------- //

void KinanimExporter::Content::WriteFrames(std::ostream* stream, FFrameData* frames, unsigned short frameCount, bool hasBlendshape, unsigned short startFrameId)
{
	for (unsigned short i = 0; i < frameCount; i++)
	{
		WriteFrame(stream, i + startFrameId, frames[i], hasBlendshape);
	}
}

void KinanimExporter::Content::WriteFrame(std::ostream* stream, unsigned short i, FFrameData frameData, bool hasBlendshape)
{
	bool hasTransform = (static_cast<uint8>(frameData.TransformDeclarationFlag) != 0);
	bool frameHadBlend = (static_cast<uint8>(frameData.BlendshapeDeclarationFlag) != 0);

	if (!hasTransform && (!hasBlendshape || !frameHadBlend)) //Don't write empty frames
		return;

	Frame::WriteFrameId(stream, i);
	Frame::WriteTransformDeclarationFlag(stream, frameData.TransformDeclarationFlag);
	if (hasTransform)
		Frame::WriteTransforms(stream, frameData.Transforms.GetData(), frameData.TransformDeclarationFlag);

	if (!hasBlendshape)  //Don't write empty frames
		return;

	Frame::WriteBlendshapeDeclarationFlag(stream, frameData.BlendshapeDeclarationFlag);
	if (frameHadBlend)
		Frame::WriteBlendshapes(stream, frameData.Blendshapes.GetData(), frameData.BlendshapeDeclarationFlag);
}

// ------------------------------- //
//              FRAME              //
// ------------------------------- //

/// <remarks>Remark, this method is private since transforms are declared in a specific order with a fixed count</remarks>
static void WriteTransform(std::ostream* stream, FTransformData transformData)
{
	short temp;
	if (transformData.bHasRotation)
	{
		temp = CustomHalf::To(transformData.Rotation.X);
		WRITE_STREAM(stream, temp);
		temp = CustomHalf::To(transformData.Rotation.Y);
		WRITE_STREAM(stream, temp);
		temp = CustomHalf::To(transformData.Rotation.Z);
		WRITE_STREAM(stream, temp);
		temp = CustomHalf::To(transformData.Rotation.W);
		WRITE_STREAM(stream, temp);
	}
	if (transformData.bHasPosition)
	{
		temp = CustomHalf::To(transformData.Position.X);
		WRITE_STREAM(stream, temp);
		temp = CustomHalf::To(transformData.Position.Y);
		WRITE_STREAM(stream, temp);
		temp = CustomHalf::To(transformData.Position.Z);
		WRITE_STREAM(stream, temp);
	}
	if (transformData.bHasScale)
	{
		temp = CustomHalf::To(transformData.Scale.X);
		WRITE_STREAM(stream, temp);
		temp = CustomHalf::To(transformData.Scale.Y);
		WRITE_STREAM(stream, temp);
		temp = CustomHalf::To(transformData.Scale.Z);
		WRITE_STREAM(stream, temp);
	}
}

void KinanimExporter::Content::Frame::WriteFrameId(std::ostream* stream, unsigned short i)
{
	WRITE_STREAM(stream, i);
}

void KinanimExporter::Content::Frame::WriteTransformDeclarationFlag(std::ostream* stream, ETransformDeclarationFlag transformDeclarationFlag)
{
	WRITE_STREAM(stream, transformDeclarationFlag);
}

void KinanimExporter::Content::Frame::WriteTransforms(std::ostream* stream,
	FTransformData transforms[static_cast<unsigned long>(EKinanimTransform::KT_Count)],
	ETransformDeclarationFlag transformDeclarationFlag)
{
	for (uint8 i = 0; i < static_cast<uint8>(EKinanimTransform::KT_Count); i++)
	{
		if (EnumFlagUtils::ContainFlag(static_cast<uint64>(transformDeclarationFlag), i))
			WriteTransform(stream, transforms[i]);
	}
}

void KinanimExporter::Content::Frame::WriteBlendshapeDeclarationFlag(std::ostream* stream, EBlendshapeDeclarationFlag blendshapeDeclarationFlag)
{
	WRITE_STREAM(stream, blendshapeDeclarationFlag);
}

void KinanimExporter::Content::Frame::WriteBlendshapes(std::ostream* stream,
	float blendshapes[static_cast<unsigned long>(EKinanimBlendshape::KB_Count)],
	EBlendshapeDeclarationFlag blendshapeDeclarationFlag)
{
	short temp;
	for (uint8 i = 0; i < static_cast<uint8>(EKinanimBlendshape::KB_Count); i++)
	{
		if (EnumFlagUtils::ContainFlag(static_cast<uint64>(blendshapeDeclarationFlag), i))
		{
			temp = CustomHalf::To(blendshapes[i]);
			WRITE_STREAM(stream, temp);
		}
	}
}