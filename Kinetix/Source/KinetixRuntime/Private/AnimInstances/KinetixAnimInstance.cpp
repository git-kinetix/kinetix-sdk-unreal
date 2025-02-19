// Copyright Kinetix. All Rights Reserved.


#include "AnimInstances/KinetixAnimInstance.h"

#include "Kismet/KismetSystemLibrary.h"

void UKinetixAnimInstance::SendNetworkedPose_Implementation()
{
	IKinetixSamplerAnimationInterface::SendNetworkedPose_Implementation();

}

void UKinetixAnimInstance::NativeSendNetworkedPose(const TArray<FName>& BoneNames, const TArray<FTransform>& BoneTransforms)
{
	BP_SendNetworkedPose(BoneNames, BoneTransforms);
}

void UKinetixAnimInstance::NativeSetNetworkMode(bool bNetworkMode)
{
	BP_SetNetworkMode(bNetworkMode);
}
