// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/KinetixSamplerAnimationInterface.h"
#include "KinetixAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)	
class KINETIXRUNTIME_API UKinetixAnimInstance
	: public UAnimInstance, public IKinetixSamplerAnimationInterface
{
	GENERATED_BODY()

public:
#pragma region IKinetixSamplerAnimationInterface
	virtual void SendNetworkedPose_Implementation() override;
#pragma endregion IKinetixSamplerAnimationInterface

	void NativeSetNetworkMode(bool bNetworkedMode);

	UFUNCTION(BlueprintImplementableEvent, Category="Kinetix|Network", meta=(DisplayName="SetNetworkMode"))
	void BP_SetNetworkMode(bool bNetworkedMode);
	
	void NativeSendNetworkedPose(const TArray<FName>& BoneNames, const TArray<FTransform>& BoneTransforms);
	
	UFUNCTION(BlueprintImplementableEvent, Category="Kinetix|Network", meta=(DisplayName="SendNetworkPose"))
	void BP_SendNetworkedPose(const TArray<FName>& BoneNames, const TArray<FTransform>& BoneTransforms);
};
