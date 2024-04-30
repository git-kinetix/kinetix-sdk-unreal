// Copyright Kinetix. All Rights Reserved.


#include "KinanimWrapper/KinanimCurveCompressionCodec.h"

#include "Kismet/KismetSystemLibrary.h"

void UKinanimCurveCompressionCodec::DecompressCurves(const FCompressedAnimSequence& AnimSeq, FBlendedCurve& Curves,
                                                     float CurrentTime) const
{
	if (!AnimSequence)
	{
		return;
	}
#if !WITH_EDITOR
	AnimSequence->GetCurveData().EvaluateCurveData(Curves, CurrentTime);
#endif
}

float UKinanimCurveCompressionCodec::DecompressCurve(const FCompressedAnimSequence& AnimSeq,
                                                     SmartName::UID_Type CurveUID, float CurrentTime) const
{
	if (!AnimSequence)
	{
		return 0.f;
	}

#if !WITH_EDITOR
	return static_cast<const FFloatCurve*>(AnimSequence->GetCurveData().GetCurveData(CurveUID))->Evaluate(CurrentTime);
#else
return 0.f;
#endif
}
