// Copyright Kinetix. All Rights Reserved.

#pragma once

#include <ostream>

#include "CoreMinimal.h"
#include "IKinanimCompression.h"
#include "KinanimData.h"

namespace KinanimExporter
{
	void WriteFile(std::ostream * stream, FKinanimData* data, IKinanimCompression * compression = nullptr, bool bClone = true);
	void WriteFileHeaderContent(std::ostream * stream, FKinanimHeader * header, FKinanimContent * content);
	void WriteHeader(std::ostream * stream, FKinanimHeader * header);
	void OverrideHeader(std::ostream* stream, FKinanimHeader* header, unsigned long long fileStartPosition = 0);
	void WriteContent(std::ostream * stream, FKinanimContent * content, FKinanimHeader * referenceHeader);
	
	namespace Header
	{
		void WriteVersion(std::ostream* stream, unsigned short version);
		void WriteFrameRate(std::ostream* stream, float frameRate);
		void WriteFrameCountAndSizes(std::ostream* stream, unsigned short frameCount, unsigned short* frameSizes);
		void WriteKeyTypes(std::ostream* stream, EKeyType keyTypes[static_cast<unsigned long>(EKinanimTransform::KT_Count)]);
		void WriteHasBlendshape(std::ostream* stream, bool bHasBlendshape);
		void WriteEndHeader(std::ostream * stream);
	}

	namespace Content
	{
		void WriteFrames(std::ostream* stream, FFrameData* frames, unsigned short frameCount, bool hasBlendshape,
		                 unsigned short startFrameId = 0);
		void WriteFrame(std::ostream* stream, unsigned short i, FFrameData frameData, bool hasBlendshape);

		namespace Frame
		{
			 void WriteFrameId(std::ostream * stream, unsigned short i);
			 void WriteTransformDeclarationFlag(std::ostream * stream,
			                                    ETransformDeclarationFlag transformDeclarationFlag);
			 void WriteTransforms(std::ostream * stream, FTransformData transforms[static_cast<unsigned long>(EKinanimTransform::KT_Count)],
			                      ETransformDeclarationFlag transformDeclarationFlag);
			 void WriteBlendshapeDeclarationFlag(std::ostream * stream,
			                                     EBlendshapeDeclarationFlag BlendshapeDeclarationFlag);
			 void WriteBlendshapes(std::ostream * stream, float blendshapes[static_cast<unsigned long>(EKinanimBlendshape::KB_Count)],
			                       EBlendshapeDeclarationFlag blendshapeDeclarationFlag);
		
		}
	}
}
