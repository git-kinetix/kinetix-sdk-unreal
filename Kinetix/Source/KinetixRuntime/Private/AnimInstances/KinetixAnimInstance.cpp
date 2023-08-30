// Copyright Kinetix. All Rights Reserved.


#include "AnimInstances/KinetixAnimInstance.h"

#include "Kismet/KismetSystemLibrary.h"

void UKinetixAnimInstance::SendNetworkedPose_Implementation()
{
	IKinetixSamplerAnimationInterface::SendNetworkedPose_Implementation();

}

void UKinetixAnimInstance::NativeSendNetworkedPose()
{
	UKismetSystemLibrary::PrintString(this, TEXT("NativeSendNetworkedPose"));
	BP_SendNetworkedPose();
}
