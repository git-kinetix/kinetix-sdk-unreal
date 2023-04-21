// // Copyright Kinetix. All Rights Reserved.


#include "Core/Animation/KinetixAnimation.h"

void UKinetixAnimation::RegisterLocalPlayerAnimationInstance(UAnimInstance* InAnimInstance)
{
	if (!IsValid(InAnimInstance))
		return;
	
	AnimInstance = InAnimInstance;

	OnRegisterLocalPlayer.Broadcast();
}

void UKinetixAnimation::SetReferenceSkeletalMesh(USkeletalMesh* InSkeletalMesh)
{
	ReferenceSkeletalMesh = InSkeletalMesh;
}
