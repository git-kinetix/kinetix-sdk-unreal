// Copyright Kinetix. All Rights Reserved.


#include "KinanimImporter.h"

#include "CustomHalf.h"
#include <stdexcept>
#include <sstream>

#define READ_STREAM(stream, variable) stream->read(reinterpret_cast<char*>(&variable), sizeof(variable))

#define THROW_STR_FORMAT(string, errorType) std::ostringstream ss;\
											ss << string;\
											throw errorType(ss.str());

#define EXCEPTION_UNKNOWN_ID(a) "ID" << a << "is not a field ID"
#define EXCEPTION_MISSALIGNED_FRAME_ID(a,b) "InvalidDataException: Trying to load frame '" << a << "' but highest imported frame is at '" << b << "'"
#define EXCEPTION_POINTER_MISSMATCHING(index, _size, _pointerEnd, _pointer) "Pointer missmatching 'KinanimHeader.FrameSizes' property when reading frame '" << index << "'. Expected: " <<_size << " but moved by " << (_pointerEnd - _pointer);

long KinanimImporter::GetHighestImportedFrame() const
{
	return _highestImportedFrame;
}

FKinanimData* KinanimImporter::GetResult() const
{
	return result;
}

FKinanimHeader* KinanimImporter::GetUncompressedHeader()
{
	return _uncompressedHeader;
}

KinanimImporter::KinanimImporter(IKinanimCompression* compression)
{
	result = new FKinanimData();
	KinanimPointerCollector::CollectPointer((IDisposable*)result);

	this->compression = compression;
	KinanimPointerCollector::CollectPointer((IDisposable*)compression);

	if (compression != nullptr)
		compression->InitTarget(result);
}

KinanimImporter::~KinanimImporter()
{
	if (result != nullptr && KinanimPointerCollector::ReleasePointer((IDisposable*)result))
	{
		delete result;
	}

	if (_uncompressedHeader != nullptr && KinanimPointerCollector::ReleasePointer((IDisposable*)_uncompressedHeader))
	{ 
		delete _uncompressedHeader;
	}
	
	if (compression != nullptr && KinanimPointerCollector::ReleasePointer((IDisposable*)compression))
	{ 
		delete compression;
	}
}

FKinanimHeader* KinanimImporter::ReleaseUncompressedHeader()
{
	FKinanimHeader* toReturn = _uncompressedHeader;
	_uncompressedHeader = nullptr;
	return toReturn;
}

FKinanimData* KinanimImporter::ReleaseResult()
{
	FKinanimData* toReturn = result;
	result = nullptr;
	return toReturn;
}

void KinanimImporter::ReadHeader(std::istream* reader)
{
	short index = 0;
	while (!reader->eof())
	{
		READ_STREAM(reader, index);
		// char indexChr = static_cast<char>(index);
		// reader->read(&indexChr, sizeof(index));

		if (index < 0) //End header
			break;

		switch (index)
		{
		case KinanimID::FIELD_ID_VERSION:
			ReadVersion(reader);
			break;
		case KinanimID::FIELD_ID_FRAMERATE:
			ReadFrameRate(reader);
			break;
		case KinanimID::FIELD_ID_FRAMECOUNT:
			ReadFrameCountAndSizes(reader);
			break;
		case KinanimID::FIELD_ID_KEYTYPES:
			ReadKeyTypes(reader);
			break;
		case KinanimID::FIELD_ID_HASBLENDSHAPES:
			ReadHasBlendshapes(reader);
			break;
#ifdef DEBUG
		default:
			THROW_STR_FORMAT(EXCEPTION_UNKNOWN_ID(index), std::logic_error)
#endif
		}

	}
	result->Header->binarySize = static_cast<long>(reader->tellg()) + 1l; //+1 cuz we go from a position to a length

	_uncompressedHeader = result->Header->Clone();
	KinanimPointerCollector::CollectPointer((IDisposable*)_uncompressedHeader);
}

void KinanimImporter::ReadFrames(std::istream* reader)
{
	int minFrame = (compression != nullptr ? compression->GetMaxUncompressedFrame() : _highestImportedFrame) + 1;
	unsigned short latestFrame = result->Header->GetFrameCount() - 1;

	std::streampos pos = reader->tellg();
	std::streamsize len = reader->seekg(0, std::ios_base::end).tellg(); //Go to the end, get position
	reader->seekg(pos, std::ios_base::beg); //Go to back to position 

	unsigned short frameCountToImport = 0;
	while (
			_highestImportedFrame < latestFrame && //Not the latest frame
			(long long)reader->tellg() + result->Header->frameSizes[_highestImportedFrame + 1l] <= (long long)len //don't overflow stream
	)
	{
		ReadFrame(reader);
		++frameCountToImport;
	}

	if (compression != nullptr)
		compression->DecompressFrame(frameCountToImport);

	long maxFrame = (compression != nullptr) ? compression->GetMaxUncompressedFrame() : _highestImportedFrame;
	ComputeUncompressedFrameSize(minFrame, maxFrame);
}

void KinanimImporter::ReadFrame(std::istream* reader)
{
	std::streampos _pointer = reader->tellg();

	unsigned short index = 0;
	READ_STREAM(reader, index);

	if (index > _highestImportedFrame)
		_highestImportedFrame = index;
	else 
	{
		THROW_STR_FORMAT(EXCEPTION_MISSALIGNED_FRAME_ID(index, _highestImportedFrame), std::invalid_argument);
	}

	unsigned short _size = result->Header->frameSizes[index];

	FFrameData frameData = {};
	
	unsigned long long ullStreamValue = 0;
	READ_STREAM(reader, ullStreamValue);

	frameData.TransformDeclarationFlag = static_cast<ETransformDeclarationFlag>(ullStreamValue);
	
	ReadTransform(&frameData, reader);
	if (result->Header->hasBlendshapes)
	{
		READ_STREAM(reader, ullStreamValue);
		frameData.BlendshapeDeclarationFlag =  static_cast<EBlendshapeDeclarationFlag>(ullStreamValue);

		ReadBlendshape(&frameData, reader);
	}

	result->Content->frames[index] = frameData;

	//Check if we read too much stuff 
	std::streampos _pointerEnd = reader->tellg();
	if (_pointerEnd - _pointer != _size)
	{
		THROW_STR_FORMAT(EXCEPTION_POINTER_MISSMATCHING(index, _size, _pointerEnd, _pointer), std::out_of_range)
	}
}

void KinanimImporter::ReadTransform(FFrameData* frameData, std::istream* reader)
{
	ETransformDeclarationFlag declarationFlag = frameData->TransformDeclarationFlag;

	if (static_cast<uint64>(declarationFlag) == 0)
		return;

	short i16StreamValue = 0;
	FTransformData transform;
	EKeyType keyType;
	for (uint8 i = 0; i < static_cast<uint8>(EKinanimTransform::KT_Count); i++)
	{
		if ((static_cast<uint64>(declarationFlag) & (1ull << i)) == 0) 
		{
			frameData->Transforms[i] =
			{
				FVector3f{0,0,0},
				FVector4f{0,0,0,1},
				FVector3f{1,1,1},
				false,
				false,
				false,
			};
			continue;
		}

		transform = {
			FVector3f{0,0,0},
			FVector4f{0,0,0,1},
			FVector3f{1,1,1},
			false,
			false,
			false,
		};

		keyType = result->Header->KeyTypes[i];
		if ((static_cast<uint8>(keyType) & static_cast<uint8>(EKeyType::KT_Rotation)) != 0)
		{
			transform.bHasRotation = true;

			READ_STREAM(reader, i16StreamValue);
			transform.Rotation.X = CustomHalf::From(i16StreamValue);

			READ_STREAM(reader, i16StreamValue);
			transform.Rotation.Y = CustomHalf::From(i16StreamValue);

			READ_STREAM(reader, i16StreamValue);
			transform.Rotation.Z = CustomHalf::From(i16StreamValue);

			READ_STREAM(reader, i16StreamValue);
			transform.Rotation.W = CustomHalf::From(i16StreamValue);
		}

		if ((static_cast<uint8>(keyType) & static_cast<uint8>(EKeyType::KT_Position)) != 0)
		{
			transform.bHasPosition = true;

			READ_STREAM(reader, i16StreamValue);
			transform.Position.X = CustomHalf::From(i16StreamValue);

			READ_STREAM(reader, i16StreamValue);
			transform.Position.Y = CustomHalf::From(i16StreamValue);

			READ_STREAM(reader, i16StreamValue);
			transform.Position.Z = CustomHalf::From(i16StreamValue);
		}

		if ((static_cast<uint8>(keyType) & static_cast<uint8>(EKeyType::KT_Scale)) != 0)
		{
			transform.bHasScale = true;

			READ_STREAM(reader, i16StreamValue);
			transform.Scale.X = CustomHalf::From(i16StreamValue);

			READ_STREAM(reader, i16StreamValue);
			transform.Scale.Y = CustomHalf::From(i16StreamValue);

			READ_STREAM(reader, i16StreamValue);
			transform.Scale.Z = CustomHalf::From(i16StreamValue);
		}

		frameData->Transforms[i] = transform;
	}
}

void KinanimImporter::ReadBlendshape(FFrameData* frameData, std::istream* reader)
{
	EBlendshapeDeclarationFlag declarationFlag = frameData->BlendshapeDeclarationFlag;

	if (static_cast<uint64>(declarationFlag) == 0)
		return;

	short i16StreamValue = 0;
	for (uint8 i = 0; i < static_cast<uint8>(EKinanimBlendshape::KB_Count); i++)
	{
		if ((static_cast<uint64>(declarationFlag) & (1ull << i)) == 0)
			continue;

		READ_STREAM(reader, i16StreamValue);
		frameData->Blendshapes[i] = CustomHalf::From(i16StreamValue);
	}
}

void KinanimImporter::ReadVersion(std::istream* reader)
{
	READ_STREAM(reader, result->Header->version);
}

void KinanimImporter::ReadFrameRate(std::istream* reader)
{
	READ_STREAM(reader, result->Header->frameRate);
}

void KinanimImporter::ReadFrameCountAndSizes(std::istream* reader)
{
	unsigned short frameCount = 0;
	READ_STREAM(reader, frameCount);

	result->Header->SetFrameCount(frameCount);
	result->Content->InitFrameCount(frameCount);
	for (int i = 0; i < frameCount; i++)
	{
		READ_STREAM(reader, result->Header->frameSizes[i]);
	}
}

void KinanimImporter::ReadKeyTypes(std::istream* reader)
{
	unsigned char ui8StreamValue = 0;
	unsigned char size = 0;
	READ_STREAM(reader, size);

	for (int i = 0; i < size; i++)
	{
		READ_STREAM(reader, ui8StreamValue);
		result->Header->KeyTypes[i] = static_cast<EKeyType>(ui8StreamValue);
	}
}

void KinanimImporter::ReadHasBlendshapes(std::istream* reader)
{
	READ_STREAM(reader, result->Header->hasBlendshapes);
}

void KinanimImporter::ComputeUncompressedFrameSize(int minFrame, int maxFrame)
{
	for (int i = minFrame; i <= maxFrame; i++)
	{
		//Recompute frame size
		_uncompressedHeader->frameSizes[i] = result->CalculateFrameSize(result->Content->frames[i]);
	}
}

void KinanimImporter::ReadFile(std::istream* reader)
{
	ReadHeader(reader);
	
	unsigned short frameReadCount = 0;
	unsigned short frameCount = result->Header->GetFrameCount();
	for (size_t i = 0; i < frameCount; i++)
	{
		if (result->Header->frameSizes[i] != 0)
			++frameReadCount;
	}


	for (int i = 0; i < frameReadCount; i++)
	{
		ReadFrame(reader);
	}
	if (compression != nullptr)
		compression->DecompressFrame(frameCount);
	ComputeUncompressedFrameSize(0, frameReadCount - 1);
}

