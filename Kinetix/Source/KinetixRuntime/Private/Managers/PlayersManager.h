// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/KinetixSubcoreInterface.h"

class UKinetixCharacterComponent;
class FPlayerManager;
/**
 * 
 */
class FPlayersManager
{
public:
	FPlayersManager();
	~FPlayersManager();

	static FPlayersManager& Get();
	
	void Initialize(FKinetixCoreConfiguration& InConfig);

	const TArray<FPlayerManager*>& GetPlayers() const;
	
#pragma region Registering

	void AddPlayerCharacterComponent(UAnimInstance* InAnimInstance, FGuid& OutGuid);

	void UnregisterLocalPlayer();

	void UnregisterPlayer(uint16 InPlayerIndex);
	void GetPlayerManager(const FGuid& InPlayer, FPlayerManager*& PlayerManager);

	void UnregisterPlayer(FGuid& InPlayer);
	
#pragma endregion

	void PlayAnimation(
		uint16 InPlayerIndex,const FAnimationID& InAnimationIDToPlay,
		const FOnPlayedKinetixAnimationLocalPlayer& OnSuccess);
	
	void PlayAnimation(FGuid& InPlayerGUID,const FAnimationID& InAnimationIDToPlay,
		const FOnPlayedKinetixAnimationLocalPlayer& OnSuccess);

	UKinetixCharacterComponent* GetLocalKCC();

#pragma region Local Player

	void SetLocalPlayer(uint16 InIndex);

	void SetLocalPlayer(const FGuid& InID);
	
#pragma endregion

#pragma region Events

	void RegisterLocalPlayerEvent();
	void UnregisterLocalPlayerEvent();
	void AnimationStartOnLocalPlayerAnimator();
	void AnimationEndOnLocalPlayerAnimator();
	
#pragma endregion

private:

	static TUniquePtr<FPlayersManager> Instance;
	
	bool bPlayAutomaticallyOnAnimInstances;

	TArray<FPlayerManager*> Players;
	FPlayerManager* LocalPlayer;
};
