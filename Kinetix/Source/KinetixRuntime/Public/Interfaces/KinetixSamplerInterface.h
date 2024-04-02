// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KinetixSamplerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKinetixSamplerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KINETIXRUNTIME_API IKinetixSamplerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category="Kinetix|Animation")
	void PlayAnimation(const FAnimationID& InID, const FString& AvatarID, UAnimSequence* InAnimSequence);

	UFUNCTION(BlueprintNativeEvent, Category="Kinetix|Animation")
	void StopAnimation();
	
};
