// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KinetixGameModeInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPlayerJoined, uint8, NumPlayers);

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UKinetixGameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KINETIXRUNTIME_API IKinetixGameModeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Kinetix|Networking")
	void RegisterAndCallOnPlayerJoined(const FOnPlayerJoined& Callback);
	
};
