// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinetixNetworkConfiguration.h"
#include "UObject/NoExportTypes.h"
#include "KinetixNetwork.generated.h"

class UKinetixCharacterComponent;
/**
 * 
 */
UCLASS()
class KINETIXRUNTIME_API UKinetixNetwork : public UObject
{
	GENERATED_BODY()

	/**
	 * @brief Set the current Kinetix Network Configuration
	 * @param InNetworkConfiguration Reference to the configuration we want
	 */
	UFUNCTION(BlueprintCallable, Category="Kinetix|Network")
	void SetConfiguration(FKinetixNetworkConfiguration& InNetworkConfiguration);

};
