// Copyright Kinetix. All Rights Reserved.


#include "GameModes/MultiplayerDemoGameMode.h"

AMultiplayerDemoGameMode::AMultiplayerDemoGameMode()
	: MaxNumPlayers(5)
{
}

void AMultiplayerDemoGameMode::RegisterAndCallOnPlayerJoined_Implementation(const FOnPlayerJoined& Callback)
{
	IKinetixGameModeInterface::RegisterAndCallOnPlayerJoined_Implementation(Callback);

	Callbacks.AddUnique(Callback);
	NotifyOnPlayerJoined();
}

void AMultiplayerDemoGameMode::NotifyOnPlayerJoined()
{
	uint8 CurrentNumPlayers = GetNumPlayers();
	for (int i = 0; i < Callbacks.Num(); ++i)
	{
		Callbacks[i].ExecuteIfBound(CurrentNumPlayers);
	}
}

void AMultiplayerDemoGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	NotifyOnPlayerJoined();
}
