// Copyright Kinetix. All Rights Reserved.


#include "KinanimWrapper.h"

#include "InterpoCompression.h"
#include "KinanimExporter.h"
#include "KinanimImporter.h"

void KinanimWrapper::KinanimExporter_WriteFile(void* /* ostream* */ stream, void* /* KinanimData* */ data,
                                               void* /* IKinanimCompression* */ compression, bool clone)
{
	KinanimExporter::WriteFile((std::ostream*)stream, (FKinanimData*)data, (IKinanimCompression*)compression, clone);
};

void KinanimWrapper::KinanimExporter_WriteFileHeaderContent(void* /* ostream* */ stream,
                                                            void* /* KinanimHeader* */ header,
                                                            void* /* KinanimContent* */ content)
{
	KinanimExporter::WriteFileHeaderContent((std::ostream*)stream, (FKinanimHeader*)header, (FKinanimContent*)content);
};

void KinanimWrapper::KinanimExporter_WriteHeader(void* /* ostream* */ stream, void* /* KinanimHeader* */ header)
{
	KinanimExporter::WriteHeader((std::ostream*)stream, (FKinanimHeader*)header);
};

void KinanimWrapper::KinanimExporter_OverrideHeader(void* /* ostream* */ stream, void* /* KinanimHeader* */ header,
                                                    unsigned long long fileStartPosition)
{
	KinanimExporter::OverrideHeader((std::ostream*)stream, (FKinanimHeader*)header, fileStartPosition);
};

void KinanimWrapper::KinanimExporter_WriteContent(void* /* ostream* */ stream, void* /* KinanimContent* */ content,
                                                  void* /* KinanimHeader* */ referenceHeader)
{
	KinanimExporter::WriteContent((std::ostream*)stream, (FKinanimContent*)content, (FKinanimHeader*)referenceHeader);
};

void KinanimWrapper::KinanimExporter_Header_WriteVersion(void* /* ostream* */ stream, unsigned short version)
{
	KinanimExporter::Header::WriteVersion((std::ostream*)stream, version);
};

void KinanimWrapper::KinanimExporter_Header_WriteFrameRate(void* /* ostream* */ stream, float frameRate)
{
	KinanimExporter::Header::WriteFrameRate((std::ostream*)stream, frameRate);
};

void KinanimWrapper::KinanimExporter_Header_WriteFrameCountAndSizes(void* /* ostream* */ stream,
                                                                    unsigned short frameCount,
                                                                    unsigned short* frameSizes)
{
	KinanimExporter::Header::WriteFrameCountAndSizes((std::ostream*)stream, frameCount, frameSizes);
};

void KinanimWrapper::KinanimExporter_Header_WriteKeyTypes(void* /* ostream* */ stream, EKeyType keyTypes[55])
{
	KinanimExporter::Header::WriteKeyTypes((std::ostream*)stream, keyTypes);
};

void KinanimWrapper::KinanimExporter_Header_WriteHasBlendshape(void* /* ostream* */ stream, bool hasBlendshape)
{
	KinanimExporter::Header::WriteHasBlendshape((std::ostream*)stream, hasBlendshape);
};

void KinanimWrapper::KinanimExporter_Header_WriteEndHeader(void* /* ostream* */ stream)
{
	KinanimExporter::Header::WriteEndHeader((std::ostream*)stream);
};

void KinanimWrapper::KinanimExporter_Content_WriteFrames(void* /* ostream* */ stream, FFrameData* frames,
                                                         unsigned short frameCount, bool hasBlendshape,
                                                         unsigned short startFrameId)
{
	KinanimExporter::Content::WriteFrames((std::ostream*)stream, frames, frameCount, hasBlendshape, startFrameId);
};

void KinanimWrapper::KinanimExporter_Content_WriteFrame(void* /* ostream* */ stream, unsigned short i,
                                                        FFrameData frameData, bool hasBlendshape)
{
	KinanimExporter::Content::WriteFrame((std::ostream*)stream, i, frameData, hasBlendshape);
};

void KinanimWrapper::KinanimExporter_Content_Frame_WriteFrameId(void* /* ostream* */ stream, unsigned short i)
{
	KinanimExporter::Content::Frame::WriteFrameId((std::ostream*)stream, i);
};

void KinanimWrapper::KinanimExporter_Content_Frame_WriteTransformDeclarationFlag(
	void* /* ostream* */ stream, ETransformDeclarationFlag transformDeclarationFlag)
{
	KinanimExporter::Content::Frame::WriteTransformDeclarationFlag((std::ostream*)stream, transformDeclarationFlag);
};

void KinanimWrapper::KinanimExporter_Content_Frame_WriteTransforms(void* /* ostream* */ stream,
                                                                   FTransformData transforms[55],
                                                                   ETransformDeclarationFlag transformDeclarationFlag)
{
	KinanimExporter::Content::Frame::WriteTransforms((std::ostream*)stream, transforms, transformDeclarationFlag);
};

void KinanimWrapper::KinanimExporter_Content_Frame_WriteBlendshapeDeclarationFlag(
	void* /* ostream* */ stream, EBlendshapeDeclarationFlag blendshapeDeclarationFlag)
{
	KinanimExporter::Content::Frame::WriteBlendshapeDeclarationFlag((std::ostream*)stream, blendshapeDeclarationFlag);
};

void KinanimWrapper::KinanimExporter_Content_Frame_WriteBlendshapes(void* /* ostream* */ stream, float blendshapes[53],
                                                                    EBlendshapeDeclarationFlag
                                                                    blendshapeDeclarationFlag)
{
	KinanimExporter::Content::Frame::WriteBlendshapes((std::ostream*)stream, blendshapes, blendshapeDeclarationFlag);
};

void KinanimWrapper::KinanimHeader_Set_version(void* _this_, unsigned short _value_)
{
	((FKinanimHeader*)_this_)->version = _value_;
};

unsigned short KinanimWrapper::KinanimHeader_Get_version(void* _this_) { return ((FKinanimHeader*)_this_)->version; };

void KinanimWrapper::KinanimHeader_Set_frameRate(void* _this_, float _value_)
{
	((FKinanimHeader*)_this_)->frameRate = _value_;
};

float KinanimWrapper::KinanimHeader_Get_frameRate(void* _this_) { return ((FKinanimHeader*)_this_)->frameRate; };

void KinanimWrapper::KinanimHeader_Set_frameSizes(void* _this_, unsigned short _value_, unsigned int _index_)
{
	((FKinanimHeader*)_this_)->frameSizes[_index_] = _value_;
};

unsigned short KinanimWrapper::KinanimHeader_Get_frameSizes(void* _this_, unsigned int _index_)
{
	return ((FKinanimHeader*)_this_)->frameSizes[_index_];
};

void KinanimWrapper::KinanimHeader_Set_binarySize(void* _this_, int _value_)
{
	((FKinanimHeader*)_this_)->binarySize = _value_;
};

int KinanimWrapper::KinanimHeader_Get_binarySize(void* _this_) { return ((FKinanimHeader*)_this_)->binarySize; };

void KinanimWrapper::KinanimHeader_Set_keyTypes(void* _this_, EKeyType _value_, unsigned int _index_)
{
	((FKinanimHeader*)_this_)->KeyTypes[_index_] = _value_;
};

EKeyType KinanimWrapper::KinanimHeader_Get_keyTypes(void* _this_, unsigned int _index_)
{
	return ((FKinanimHeader*)_this_)->KeyTypes[_index_];
};

void KinanimWrapper::KinanimHeader_Set_hasBlendshapes(void* _this_, bool _value_)
{
	((FKinanimHeader*)_this_)->hasBlendshapes = _value_;
};

bool KinanimWrapper::KinanimHeader_Get_hasBlendshapes(void* _this_)
{
	return ((FKinanimHeader*)_this_)->hasBlendshapes;
};

unsigned short KinanimWrapper::KinanimHeader_GetFrameCount(void* /* KinanimHeader */ _this_)
{
	return ((FKinanimHeader*)_this_)->GetFrameCount();
};

void KinanimWrapper::KinanimHeader_SetFrameCount(void* /* KinanimHeader */ _this_, unsigned short count)
{
	((FKinanimHeader*)_this_)->SetFrameCount(count);
};

void* /* KinanimHeader* */ KinanimWrapper::KinanimHeader_Clone(void* /* KinanimHeader */ _this_)
{
	return ((FKinanimHeader*)_this_)->Clone();
};

void* KinanimWrapper::Ctor_KinanimHeader()
{
	FKinanimHeader* toReturn = new FKinanimHeader();
	KinanimPointerCollector::CollectPointer((IDisposable*)toReturn);
	return toReturn;
};

void KinanimWrapper::Delete_KinanimHeader(void* _this_)
{
	if (KinanimPointerCollector::ReleasePointer((IDisposable*)_this_)) { delete ((FKinanimHeader*)_this_); }
};

void KinanimWrapper::KinanimContent_Set_frames(void* _this_, FFrameData _value_, unsigned int _index_)
{
	((FKinanimContent*)_this_)->frames[_index_] = _value_;
};

FFrameData KinanimWrapper::KinanimContent_Get_frames(void* _this_, unsigned int _index_)
{
	FFrameData Frame = ((FKinanimContent*)_this_)->frames[_index_];
	return Frame;
};

unsigned int KinanimWrapper::KinanimContent_GetFrameCount(void* /* KinanimContent */ _this_)
{
	return ((FKinanimContent*)_this_)->GetFrameCount();
};

void KinanimWrapper::KinanimContent_InitFrameCount(void* /* KinanimContent */ _this_, unsigned short frameCount)
{
	((FKinanimContent*)_this_)->InitFrameCount(frameCount);
};

void* /* KinanimContent* */ KinanimWrapper::KinanimContent_Clone(void* /* KinanimContent */ _this_)
{
	return ((FKinanimContent*)_this_)->Clone();
};

void* KinanimWrapper::Ctor_KinanimContent()
{
	FKinanimContent* toReturn = new FKinanimContent();
	KinanimPointerCollector::CollectPointer((IDisposable*)toReturn);
	return toReturn;
};

void KinanimWrapper::Delete_KinanimContent(void* _this_)
{
	if (KinanimPointerCollector::ReleasePointer((IDisposable*)_this_)) { delete ((FKinanimContent*)_this_); }
};

const char* KinanimWrapper::KinanimData_Get_FILE_EXTENSION() { return FKinanimData::FILE_EXTENSION; };

void KinanimWrapper::KinanimData_Set_header(void* _this_, void* /* KinanimHeader* */ _value_)
{
	((FKinanimData*)_this_)->Header = (FKinanimHeader*)_value_;
};

void* /* KinanimHeader* */ KinanimWrapper::KinanimData_Get_header(void* _this_)
{
	return ((FKinanimData*)_this_)->Header;
};

void KinanimWrapper::KinanimData_Set_content(void* _this_, void* /* KinanimContent* */ _value_)
{
	((FKinanimData*)_this_)->Content = (FKinanimContent*)_value_;
};

void* /* KinanimContent* */ KinanimWrapper::KinanimData_Get_content(void* _this_)
{
	return ((FKinanimData*)_this_)->Content;
};

void KinanimWrapper::KinanimData_CalculateEveryFrameSize(void* /* KinanimData */ _this_)
{
	((FKinanimData*)_this_)->CalculateEveryFrameSize();
};

unsigned short KinanimWrapper::KinanimData_CalculateFrameSize(void* /* KinanimData */ _this_, FFrameData data)
{
	return ((FKinanimData*)_this_)->CalculateFrameSize(data);
};

void* /* KinanimData* */ KinanimWrapper::KinanimData_Clone(void* /* KinanimData */ _this_)
{
	return ((FKinanimData*)_this_)->Clone();
};

void* KinanimWrapper::Ctor_KinanimData()
{
	FKinanimData* toReturn = new FKinanimData();
	KinanimPointerCollector::CollectPointer((IDisposable*)toReturn);
	return toReturn;
};

void KinanimWrapper::Delete_KinanimData(void* _this_)
{
	if (KinanimPointerCollector::ReleasePointer((IDisposable*)_this_)) { delete ((FKinanimData*)_this_); }
};

const int KinanimWrapper::InterpoCompression_Get_DEFAULT_BATCH_SIZE()
{
	return InterpoCompression::DEFAULT_BATCH_SIZE;
};

void KinanimWrapper::InterpoCompression_Set_maxFramePerLerp(void* _this_, unsigned short _value_)
{
	((InterpoCompression*)_this_)->maxFramePerLerp = _value_;
};

unsigned short KinanimWrapper::InterpoCompression_Get_maxFramePerLerp(void* _this_)
{
	return ((InterpoCompression*)_this_)->maxFramePerLerp;
};

void KinanimWrapper::InterpoCompression_Set_threshold(void* _this_, float _value_)
{
	((InterpoCompression*)_this_)->threshold = _value_;
};

float KinanimWrapper::InterpoCompression_Get_threshold(void* _this_)
{
	return ((InterpoCompression*)_this_)->threshold;
};

void KinanimWrapper::InterpoCompression_Set_blendshapeThrshold(void* _this_, float _value_)
{
	((InterpoCompression*)_this_)->blendshapeThrshold = _value_;
};

float KinanimWrapper::InterpoCompression_Get_blendshapeThrshold(void* _this_)
{
	return ((InterpoCompression*)_this_)->blendshapeThrshold;
};

int KinanimWrapper::InterpoCompression_GetMaxUncompressedFrame(void* /* InterpoCompression */ _this_)
{
	return ((InterpoCompression*)_this_)->GetMaxUncompressedFrame();
};

int KinanimWrapper::InterpoCompression_GetMaxUncompressedTransforms(void* /* InterpoCompression */ _this_)
{
	return ((InterpoCompression*)_this_)->GetMaxUncompressedTransforms();
};

int KinanimWrapper::InterpoCompression_GetMaxUncompressedBlendshapes(void* /* InterpoCompression */ _this_)
{
	return ((InterpoCompression*)_this_)->GetMaxUncompressedBlendshapes();
};

void KinanimWrapper::InterpoCompression_InitTarget(void* /* InterpoCompression */ _this_, void* /* KinanimData* */ data)
{
	((InterpoCompression*)_this_)->InitTarget((FKinanimData*)data);
};

void KinanimWrapper::InterpoCompression_Compress(void* /* InterpoCompression */ _this_)
{
	((InterpoCompression*)_this_)->Compress();
};

void KinanimWrapper::InterpoCompression_DecompressFrame(void* /* InterpoCompression */ _this_,
                                                        unsigned short loadedFrameCount)
{
	((InterpoCompression*)_this_)->DecompressFrame(loadedFrameCount);
};

void* KinanimWrapper::Ctor_InterpoCompression(unsigned short maxFramePerLerp, float threshold, float blendshapeThrshold)
{
	InterpoCompression* toReturn = new InterpoCompression(maxFramePerLerp, threshold, blendshapeThrshold);
	KinanimPointerCollector::CollectPointer((IDisposable*)toReturn);
	return toReturn;
};

void KinanimWrapper::Delete_InterpoCompression(void* _this_)
{
	if (KinanimPointerCollector::ReleasePointer((IDisposable*)_this_)) { delete ((InterpoCompression*)_this_); }
};

void KinanimWrapper::KinanimImporter_Set_compression(void* _this_, void* /* IKinanimCompression* */ _value_)
{
	((KinanimImporter*)_this_)->compression = (IKinanimCompression*)_value_;
};

void* /* IKinanimCompression* */ KinanimWrapper::KinanimImporter_Get_compression(void* _this_)
{
	return ((KinanimImporter*)_this_)->compression;
};

int KinanimWrapper::KinanimImporter_GetHighestImportedFrame(void* /* KinanimImporter */ _this_)
{
	return ((KinanimImporter*)_this_)->GetHighestImportedFrame();
};

void* /* KinanimData* */ KinanimWrapper::KinanimImporter_GetResult(void* /* KinanimImporter */ _this_)
{
	return ((KinanimImporter*)_this_)->GetResult();
};

void* /* KinanimHeader* */ KinanimWrapper::KinanimImporter_GetUncompressedHeader(void* /* KinanimImporter */ _this_)
{
	return ((KinanimImporter*)_this_)->GetUncompressedHeader();
};

void* /* KinanimHeader* */ KinanimWrapper::KinanimImporter_ReleaseUncompressedHeader(void* /* KinanimImporter */ _this_)
{
	return ((KinanimImporter*)_this_)->ReleaseUncompressedHeader();
};

void* /* KinanimData* */ KinanimWrapper::KinanimImporter_ReleaseResult(void* /* KinanimImporter */ _this_)
{
	return ((KinanimImporter*)_this_)->ReleaseResult();
};

void KinanimWrapper::KinanimImporter_ReadHeader(void* /* KinanimImporter */ _this_, void* /* istream* */ reader)
{
	((KinanimImporter*)_this_)->ReadHeader((std::istream*)reader);
};

void KinanimWrapper::KinanimImporter_ReadFrames(void* /* KinanimImporter */ _this_, void* /* istream* */ reader)
{
	((KinanimImporter*)_this_)->ReadFrames((std::istream*)reader);
};

void KinanimWrapper::KinanimImporter_ReadFrame(void* /* KinanimImporter */ _this_, void* /* istream* */ reader)
{
	((KinanimImporter*)_this_)->ReadFrame((std::istream*)reader);
};

void KinanimWrapper::KinanimImporter_ReadTransform(void* /* KinanimImporter */ _this_, FFrameData* frameData,
                                                   void* /* istream* */ reader)
{
	((KinanimImporter*)_this_)->ReadTransform(frameData, (std::istream*)reader);
};

void KinanimWrapper::KinanimImporter_ReadBlendshape(void* /* KinanimImporter */ _this_, FFrameData* frameData,
                                                    void* /* istream* */ reader)
{
	((KinanimImporter*)_this_)->ReadBlendshape(frameData, (std::istream*)reader);
};

void KinanimWrapper::KinanimImporter_ComputeUncompressedFrameSize(void* /* KinanimImporter */ _this_, int minFrame,
                                                                  int maxFrame)
{
	((KinanimImporter*)_this_)->ComputeUncompressedFrameSize(minFrame, maxFrame);
};

void KinanimWrapper::KinanimImporter_ReadFile(void* /* KinanimImporter */ _this_, void* /* istream* */ stream)
{
	((KinanimImporter*)_this_)->ReadFile((std::istream*)stream);
};

void* KinanimWrapper::Ctor_KinanimImporter(void* /* IKinanimCompression* */ compression)
{
	KinanimImporter* toReturn = new KinanimImporter((IKinanimCompression*)compression);
	KinanimPointerCollector::CollectPointer((IDisposable*)toReturn);
	return toReturn;
};

void KinanimWrapper::Delete_KinanimImporter(void* _this_)
{
	if (KinanimPointerCollector::ReleasePointer((IDisposable*)_this_)) { delete ((KinanimImporter*)_this_); }
};
