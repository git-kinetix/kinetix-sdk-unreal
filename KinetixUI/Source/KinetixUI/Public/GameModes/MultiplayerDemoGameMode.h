// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/KinetixGameModeInterface.h"
#include "MultiplayerDemoGameMode.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class KINETIXUI_API AMultiplayerDemoGameMode
	: public AGameModeBase, public IKinetixGameModeInterface
{

	GENERATED_BODY()

public:

	AMultiplayerDemoGameMode();

	virtual void RegisterAndCallOnPlayerJoined_Implementation(const FOnPlayerJoined& Callback) override;
	
	void NotifyOnPlayerJoined();
	
	virtual void OnPostLogin(AController* NewPlayer) override;

private:

	UPROPERTY()
	TArray<FOnPlayerJoined> Callbacks;

	uint8 MaxNumPlayers; 
};
