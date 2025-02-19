// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KinetixAnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UKinetixAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KINETIXRUNTIME_API IKinetixAnimationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category="Kinetix|Animation")
	void SetKinetixAnimationPlaying(bool bInEmoteMode);
	
	UFUNCTION(BlueprintNativeEvent, Category="Kinetix|Animation")
	bool IsKinetixAnimationPlaying();
};
