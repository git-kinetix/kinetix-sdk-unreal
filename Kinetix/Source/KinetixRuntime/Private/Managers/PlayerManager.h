// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"

class UKinetixCharacterComponent;

/**
 * 
 */
class FPlayerManager
{
public:
	FPlayerManager(bool bInPlayAutomaticallyOnAnimator);
	~FPlayerManager();

	bool AddKinetixComponentAndInitialize(UAnimInstance* InAnimInstance);
	bool AddPlayerCharacterComponent(UAnimInstance* InAnimInstance);
	bool RemovePlayerCharacterComponent();

	bool IsAnimationAvailable(FAnimationID InAnimationID) const;

	bool IsEmoteUsedByPlayer(FAnimationID InAnimationID) const;

	void GetNotifiedOnAnimationReadyToPlayOnLocalAnimInstance(FAnimationID InAnimationID, TDelegate<void()> OnSucceedDelegate);
	
	void AnimationStartOnLocalPlayerAnimInstance(FAnimationID InAnimationID) const;
	void AnimationEndOnLocalPlayerAnimInstance(FAnimationID InAnimationID) const;

	TArray<FAnimationID> GetDownloadedAnimationReadyToPlay() const;

	void PlayAnimation(const FAnimationID& InAnimationID,  const FOnPlayedKinetixAnimationLocalPlayer& OnPlayedAnimation);
	void PlayAnimationQueue(TArray<FAnimationID> InAnimationIDs, bool bLoop, TDelegate<void(TArray<FAnimationID>)> OnPlayedAnimations);
	void StopAnimation();

	TObjectPtr<UKinetixCharacterComponent> GetLocalKinetixComponent() const;

	void UnregisterKinetixComponent();

#pragma region Loading

	void LoadLocalPlayerAnimation(const FAnimationID& InAnimationID,
		FString InLockID,
		const TDelegate<void()>& OnSuccess = TDelegate<void()>(),
		const TDelegate<void()>& OnFailure = TDelegate<void()>());

#pragma endregion

#pragma region Unloading
	
	void UnloadPlayerAnimation(const FAnimationID& InAnimationID);
	
#pragma endregion Unloading

private:
	void OnRegisterLocalPlayer();

public:

	const FGuid UUID; 
	
private:
	bool bPlayAutomatcallyOnAnimInstance;

	TObjectPtr<UKinetixCharacterComponent> LocalKinetixComponent;

	TMap<FAnimationID, TList<TDelegate<void()>>> CallbacksOnRetargetedAnimationIDOnLocalPlayer;
	TList<FAnimationID>* EmotesToPreload;
	TMap<FAnimationID, UAnimSequence*> DownloadedEmotesReadyToPlay;

};
