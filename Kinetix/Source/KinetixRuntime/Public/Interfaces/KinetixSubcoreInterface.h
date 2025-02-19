// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/KinetixDataLibrary.h"
#include "KinetixSubcoreInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKinetixSubcoreInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KINETIXRUNTIME_API IKinetixSubcoreInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	void Initialize(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult);
};
