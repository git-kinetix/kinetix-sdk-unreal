// Copyright Kinetix. All Rights Reserved.


#include "Managers/PlayersManager.h"

#include "PlayerManager.h"
#include "Core/Animation/KinetixAnimation.h"

TUniquePtr<FPlayersManager> FPlayersManager::Instance = nullptr;

FPlayersManager::FPlayersManager()
	: bPlayAutomaticallyOnAnimInstances(false), LocalPlayer(nullptr)
{
}

FPlayersManager::~FPlayersManager()
{
}

FPlayersManager& FPlayersManager::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeUnique<FPlayersManager>();
	}

	return *Instance;
}

void FPlayersManager::Initialize(FKinetixCoreConfiguration& InConfig)
{
	
}

const TArray<FPlayerManager*>& FPlayersManager::GetPlayers() const
{
	return Players;
}

void FPlayersManager::AddPlayerCharacterComponent(UAnimInstance* InAnimInstance, FGuid& OutGuid, FString AvatarUUID)
{
	if (!IsValid(InAnimInstance))
		return;

	FPlayerManager* NewPlayer = new FPlayerManager(true);
	NewPlayer->AddPlayerCharacterComponent(InAnimInstance, AvatarUUID);
	Players.Add(NewPlayer);
	OutGuid = NewPlayer->UUID;
	
	UE_LOG(LogKinetixAnimation, Log, TEXT("OutGuid: %s, NewPlayer.UUID %s, Pleyrs[].UUID: %s"),
		*OutGuid.ToString(EGuidFormats::DigitsWithHyphensLower),
		*NewPlayer->UUID.ToString(EGuidFormats::DigitsWithHyphensLower),
		*Players[Players.Num()-1]->UUID.ToString(EGuidFormats::DigitsWithHyphensLower));
	
}

void FPlayersManager::UnregisterLocalPlayer()
{
	if (LocalPlayer == nullptr)
		return;

	LocalPlayer->UnregisterKinetixComponent();
	Players.Remove(LocalPlayer);
	LocalPlayer = nullptr;
}

void FPlayersManager::UnregisterPlayer(uint16 InPlayerIndex)
{
	if (InPlayerIndex >= Players.Num())
		return;

	FPlayerManager* PlayerManager = Players[InPlayerIndex];
	if (PlayerManager == nullptr)
		return;
	
	PlayerManager->UnregisterKinetixComponent();
	Players.Remove(PlayerManager);
}

void FPlayersManager::GetPlayerManager(const FGuid& InPlayer, FPlayerManager*& PlayerManager)
{
	for (int i = 0; i < Players.Num(); ++i)
	{
		if (Players[i]->UUID != InPlayer)
			continue;

		PlayerManager = Players[i];
		i = Players.Num();
	}
}

void FPlayersManager::UnregisterPlayer(FGuid& InPlayer)
{
	if (!InPlayer.IsValid())
		return;
	
	FPlayerManager* PlayerManager = nullptr;

	GetPlayerManager(InPlayer, PlayerManager);

	if (PlayerManager == nullptr)
		return;

	PlayerManager->UnregisterKinetixComponent();
	Players.Remove(PlayerManager);
}

void FPlayersManager::PlayAnimation(uint16 InPlayerIndex,const FAnimationID& InAnimationIDToPlay,
	const FOnPlayedKinetixAnimationLocalPlayer& OnSuccess)
{
	if (InPlayerIndex >= Players.Num())
		return;

	Players[InPlayerIndex]->PlayAnimation(InAnimationIDToPlay, OnSuccess);
}

void FPlayersManager::PlayAnimation(FGuid& InPlayerGUID,const FAnimationID& InAnimationIDToPlay,
	const FOnPlayedKinetixAnimationLocalPlayer& OnSuccess)
{
	if (!InPlayerGUID.IsValid())
		return;
	
	FPlayerManager* PlayerManager = nullptr;

	GetPlayerManager(InPlayerGUID, PlayerManager);

	if (PlayerManager == nullptr)
		return;

	PlayerManager->PlayAnimation(InAnimationIDToPlay, OnSuccess);
}

UKinetixCharacterComponent* FPlayersManager::GetLocalKCC()
{
	return nullptr;
}

void FPlayersManager::SetLocalPlayer(uint16 InIndex)
{
	if (InIndex >= Players.Num())
		return;

	LocalPlayer = Players[InIndex];
}

void FPlayersManager::SetLocalPlayer(const FGuid& InPlayerIndex)
{
	if (!InPlayerIndex.IsValid())
		return;

	GetPlayerManager(InPlayerIndex, LocalPlayer);
}

void FPlayersManager::RegisterLocalPlayerEvent()
{
	if (LocalPlayer == nullptr)
		return;

	
}

void FPlayersManager::UnregisterLocalPlayerEvent()
{
}

void FPlayersManager::AnimationStartOnLocalPlayerAnimator()
{
}

void FPlayersManager::AnimationEndOnLocalPlayerAnimator()
{
}
