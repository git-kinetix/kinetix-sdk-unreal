// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "KinanimTypes.h"

#ifdef __cplusplus
// extern "C" {
#endif


/**
* 
*/
namespace KinanimWrapper
{
	KINANIM_API void KinanimExporter_WriteFile(void* /* ostream* */ stream, void* /* KinanimData* */ data,
	                                           void* /* IKinanimCompression* */ compression = nullptr, bool clone = 1);

	KINANIM_API void KinanimExporter_WriteFileHeaderContent(void* /* ostream* */ stream,
	                                                        void* /* KinanimHeader* */ header,
	                                                        void* /* KinanimContent* */ content);

	KINANIM_API void KinanimExporter_WriteHeader(void* /* ostream* */ stream, void* /* KinanimHeader* */ header);

	KINANIM_API void KinanimExporter_OverrideHeader(void* /* ostream* */ stream, void* /* KinanimHeader* */ header,
	                                                unsigned long long fileStartPosition = 0);

	KINANIM_API void KinanimExporter_WriteContent(void* /* ostream* */ stream, void* /* KinanimContent* */ content,
	                                              void* /* KinanimHeader* */ referenceHeader);

	KINANIM_API void KinanimExporter_Header_WriteVersion(void* /* ostream* */ stream, unsigned short version);

	KINANIM_API void KinanimExporter_Header_WriteFrameRate(void* /* ostream* */ stream, float frameRate);

	KINANIM_API void KinanimExporter_Header_WriteFrameCountAndSizes(void* /* ostream* */ stream,
	                                                                unsigned short frameCount,
	                                                                unsigned short* frameSizes);

	KINANIM_API void KinanimExporter_Header_WriteKeyTypes(void* /* ostream* */ stream, EKeyType keyTypes[55]);

	KINANIM_API void KinanimExporter_Header_WriteHasBlendshape(void* /* ostream* */ stream, bool hasBlendshape);

	KINANIM_API void KinanimExporter_Header_WriteEndHeader(void* /* ostream* */ stream);

	KINANIM_API void KinanimExporter_Content_WriteFrames(void* /* ostream* */ stream, FFrameData* frames,
	                                                     unsigned short frameCount, bool hasBlendshape,
	                                                     unsigned short startFrameId = 0);

	KINANIM_API void KinanimExporter_Content_WriteFrame(void* /* ostream* */ stream, unsigned short i,
	                                                    FFrameData frameData, bool hasBlendshape);

	KINANIM_API void KinanimExporter_Content_Frame_WriteFrameId(void* /* ostream* */ stream, unsigned short i);

	KINANIM_API void KinanimExporter_Content_Frame_WriteTransformDeclarationFlag(
		void* /* ostream* */ stream, ETransformDeclarationFlag transformDeclarationFlag);

	KINANIM_API void KinanimExporter_Content_Frame_WriteTransforms(void* /* ostream* */ stream,
	                                                               FTransformData transforms[55],
	                                                               ETransformDeclarationFlag transformDeclarationFlag);

	KINANIM_API void KinanimExporter_Content_Frame_WriteBlendshapeDeclarationFlag(
		void* /* ostream* */ stream, EBlendshapeDeclarationFlag blendshapeDeclarationFlag);

	KINANIM_API void KinanimExporter_Content_Frame_WriteBlendshapes(void* /* ostream* */ stream, float blendshapes[53],
	                                                                EBlendshapeDeclarationFlag
	                                                                blendshapeDeclarationFlag);

	///<summary >
	///Version of the file formate
	///</summary >
	KINANIM_API void KinanimHeader_Set_version(void* _this_, unsigned short _value_);

	///<summary >
	///Version of the file formate
	///</summary >
	KINANIM_API unsigned short KinanimHeader_Get_version(void* _this_);

	///<summary >
	///Play rate of frames
	///</summary >
	KINANIM_API void KinanimHeader_Set_frameRate(void* _this_, float _value_);

	///<summary >
	///Play rate of frames
	///</summary >
	KINANIM_API float KinanimHeader_Get_frameRate(void* _this_);

	///<summary >
	///Binary size of frames in the file
	///</summary >
	KINANIM_API void KinanimHeader_Set_frameSizes(void* _this_, unsigned short _value_, unsigned int _index_);

	///<summary >
	///Binary size of frames in the file
	///</summary >
	KINANIM_API unsigned short KinanimHeader_Get_frameSizes(void* _this_, unsigned int _index_);

	///<summary >
	///Size in byte of the header.<br />Retrieved during 
	///<see cref="KinanimImporter.ReadHeader(System.IO.BinaryReader)"/>.
	///</summary >
	KINANIM_API void KinanimHeader_Set_binarySize(void* _this_, int _value_);

	///<summary >
	///Size in byte of the header.<br />Retrieved during 
	///<see cref="KinanimImporter.ReadHeader(System.IO.BinaryReader)"/>.
	///</summary >
	KINANIM_API int KinanimHeader_Get_binarySize(void* _this_);

	///<summary >
	///The type of keys used
	///</summary >
	KINANIM_API void KinanimHeader_Set_keyTypes(void* _this_, EKeyType _value_, unsigned int _index_);

	///<summary >
	///The type of keys used
	///</summary >
	KINANIM_API EKeyType KinanimHeader_Get_keyTypes(void* _this_, unsigned int _index_);

	///<summary >
	///If the animation has blendshapes or not
	///</summary >
	KINANIM_API void KinanimHeader_Set_hasBlendshapes(void* _this_, bool _value_);

	///<summary >
	///If the animation has blendshapes or not
	///</summary >
	KINANIM_API bool KinanimHeader_Get_hasBlendshapes(void* _this_);

	///<summary >
	///Get the number of frames
	///</summary >
	KINANIM_API unsigned short KinanimHeader_GetFrameCount(void* /* KinanimHeader */ _this_);

	///<summary >
	///Set the number of frames
	///</summary >
	KINANIM_API void KinanimHeader_SetFrameCount(void* /* KinanimHeader */ _this_, unsigned short count);

	KINANIM_API void* /* KinanimHeader* */ KinanimHeader_Clone(void* /* KinanimHeader */ _this_);

	KINANIM_API void* /* KinanimHeader* */ Ctor_KinanimHeader();

	KINANIM_API void Delete_KinanimHeader(void* /* KinanimHeader */ _this_);

	KINANIM_API void KinanimContent_Set_frames(void* _this_, FFrameData _value_, unsigned int _index_);

	KINANIM_API FFrameData KinanimContent_Get_frames(void* _this_, unsigned int _index_);

	KINANIM_API unsigned int KinanimContent_GetFrameCount(void* /* KinanimContent */ _this_);

	KINANIM_API void KinanimContent_InitFrameCount(void* /* KinanimContent */ _this_, unsigned short frameCount);

	KINANIM_API void* /* KinanimContent* */ KinanimContent_Clone(void* /* KinanimContent */ _this_);

	KINANIM_API void* /* KinanimContent* */ Ctor_KinanimContent();

	KINANIM_API void Delete_KinanimContent(void* /* KinanimContent */ _this_);

	KINANIM_API const char* KinanimData_Get_FILE_EXTENSION();

	KINANIM_API void KinanimData_Set_header(void* _this_, void* /* KinanimHeader* */ _value_);

	KINANIM_API void* /* KinanimHeader* */ KinanimData_Get_header(void* _this_);

	KINANIM_API void KinanimData_Set_content(void* _this_, void* /* KinanimContent* */ _value_);

	KINANIM_API void* /* KinanimContent* */ KinanimData_Get_content(void* _this_);

	KINANIM_API void KinanimData_CalculateEveryFrameSize(void* /* KinanimData */ _this_);

	KINANIM_API unsigned short KinanimData_CalculateFrameSize(void* /* KinanimData */ _this_, FFrameData data);

	KINANIM_API void* /* KinanimData* */ KinanimData_Clone(void* /* KinanimData */ _this_);

	KINANIM_API void* /* KinanimData* */ Ctor_KinanimData();

	KINANIM_API void Delete_KinanimData(void* /* KinanimData */ _this_);

	///<summary >
	///Default value for maxFramePerLerp in constructor. 
	///</summary >
	KINANIM_API const int InterpoCompression_Get_DEFAULT_BATCH_SIZE();

	///<summary >
	///Maximum frame per interpolation<br /><br />For example: Interpolation from 2 to 5 has 6 frames
	///</summary >
	KINANIM_API void InterpoCompression_Set_maxFramePerLerp(void* _this_, unsigned short _value_);

	///<summary >
	///Maximum frame per interpolation<br /><br />For example: Interpolation from 2 to 5 has 6 frames
	///</summary >
	KINANIM_API unsigned short InterpoCompression_Get_maxFramePerLerp(void* _this_);

	///<summary >
	///A value between 0 and 1 that will result in more or less compression
	///</summary >
	KINANIM_API void InterpoCompression_Set_threshold(void* _this_, float _value_);

	///<summary >
	///A value between 0 and 1 that will result in more or less compression
	///</summary >
	KINANIM_API float InterpoCompression_Get_threshold(void* _this_);

	///<summary >
	///A value between 0 and 1 for checking the distance between 2 blendshape values
	///</summary >
	KINANIM_API void InterpoCompression_Set_blendshapeThrshold(void* _this_, float _value_);

	///<summary >
	///A value between 0 and 1 for checking the distance between 2 blendshape values
	///</summary >
	KINANIM_API float InterpoCompression_Get_blendshapeThrshold(void* _this_);

	///<inheritdoc />
	KINANIM_API int InterpoCompression_GetMaxUncompressedFrame(void* /* InterpoCompression */ _this_);

	///<inheritdoc />
	KINANIM_API int InterpoCompression_GetMaxUncompressedTransforms(void* /* InterpoCompression */ _this_);

	///<inheritdoc />
	KINANIM_API int InterpoCompression_GetMaxUncompressedBlendshapes(void* /* InterpoCompression */ _this_);

	///<inheritdoc />
	KINANIM_API void
	InterpoCompression_InitTarget(void* /* InterpoCompression */ _this_, void* /* KinanimData* */ data);

	///<inheritdoc />
	KINANIM_API void InterpoCompression_Compress(void* /* InterpoCompression */ _this_);

	///<inheritdoc />
	KINANIM_API void InterpoCompression_DecompressFrame(void* /* InterpoCompression */ _this_,
	                                                    unsigned short loadedFrameCount);

	///<summary >
	///Constructor of 
	///<see cref="InterpoCompression"/>
	///</summary >
	///<param name="maxFramePerLerp">
	///Maximum frame per interpolation<br /><br />For example: Interpolation from 2 to 5 has 6 frames
	///</param >
	///<param name="threshold">
	///A value between 0 and 1 that will result in more or less compression
	///</param >
	///<param name="blendshapeThrshold">
	///A value between 0 and 1 for checking the distance between 2 blendshape values
	///</param >
	KINANIM_API void* /* InterpoCompression* */ Ctor_InterpoCompression(unsigned short maxFramePerLerp = 20,
	                                                                    float threshold = 0.10000000149011612,
	                                                                    float blendshapeThrshold =
		                                                                    0.009999999776482582);

	KINANIM_API void Delete_InterpoCompression(void* /* InterpoCompression */ _this_);

	///<summary >
	///Lossy compression algorythm to use to decompress (interpolate) the kinanim
	///</summary >
	KINANIM_API void KinanimImporter_Set_compression(void* _this_, void* /* IKinanimCompression* */ _value_);

	///<summary >
	///Lossy compression algorythm to use to decompress (interpolate) the kinanim
	///</summary >
	KINANIM_API void* /* IKinanimCompression* */ KinanimImporter_Get_compression(void* _this_);

	KINANIM_API int KinanimImporter_GetHighestImportedFrame(void* /* KinanimImporter */ _this_);

	///<summary >
	///Imported data
	///</summary >
	KINANIM_API void* /* KinanimData* */ KinanimImporter_GetResult(void* /* KinanimImporter */ _this_);

	KINANIM_API void* /* KinanimHeader* */ KinanimImporter_GetUncompressedHeader(void* /* KinanimImporter */ _this_);

	///<summary >
	///Set uncompressed header to nullptr to avoid getting it deleted by the destructor
	///</summary >
	KINANIM_API void* /* KinanimHeader* */
	KinanimImporter_ReleaseUncompressedHeader(void* /* KinanimImporter */ _this_);

	///<summary >
	///Set result to nullptr to avoid getting it deleted by the destructor
	///</summary >
	///<returns >Returns the pointer before release
	///</returns >
	KINANIM_API void* /* KinanimData* */ KinanimImporter_ReleaseResult(void* /* KinanimImporter */ _this_);

	KINANIM_API void KinanimImporter_ReadHeader(void* /* KinanimImporter */ _this_, void* /* istream* */ reader);

	KINANIM_API void KinanimImporter_ReadFrames(void* /* KinanimImporter */ _this_, void* /* istream* */ reader);

	KINANIM_API void KinanimImporter_ReadFrame(void* /* KinanimImporter */ _this_, void* /* istream* */ reader);

	KINANIM_API void KinanimImporter_ReadTransform(void* /* KinanimImporter */ _this_, FFrameData* frameData,
	                                               void* /* istream* */ reader);

	KINANIM_API void KinanimImporter_ReadBlendshape(void* /* KinanimImporter */ _this_, FFrameData* frameData,
	                                                void* /* istream* */ reader);

	///<summary >
	///<see cref="UncompressedHeader"/>
	///</summary >
	///<param name="minFrame">
	///</param >
	///<param name="maxFrame">
	///</param >
	KINANIM_API void KinanimImporter_ComputeUncompressedFrameSize(void* /* KinanimImporter */ _this_, int minFrame,
	                                                              int maxFrame);

	KINANIM_API void KinanimImporter_ReadFile(void* /* KinanimImporter */ _this_, void* /* istream* */ stream);

	///<summary >
	///Import a 
	///<see cref="KinanimData"/> from a file.kinanim
	///</summary >
	///<param name="compression">Lossy compression algorythm to use to decompress (interpolate) the kinanim
	///</param >
	KINANIM_API void* /* KinanimImporter* */ Ctor_KinanimImporter(
		void* /* IKinanimCompression* */ compression = nullptr);

	KINANIM_API void Delete_KinanimImporter(void* /* KinanimImporter */ _this_);
}

#ifdef __cplusplus
// }
#endif
