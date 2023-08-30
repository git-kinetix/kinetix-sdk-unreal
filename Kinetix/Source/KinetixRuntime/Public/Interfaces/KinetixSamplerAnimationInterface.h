// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KinetixSamplerAnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UKinetixSamplerAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KINETIXRUNTIME_API IKinetixSamplerAnimationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category="Kinetix|Networking")
	void SendNetworkedPose(/*FName BoneNames, const TArray<FTransform>& BoneTransforms*/);
	
};
