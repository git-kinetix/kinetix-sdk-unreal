// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"

class UKinetixComponent;

/**
 * 
 */
class FLocalPlayerManager
{
public:
	FLocalPlayerManager(bool bInPlayAutomaticallyOnAnimator);
	~FLocalPlayerManager();
	bool AddKinetixComponentAndInitialize(UAnimInstance* InAnimInstance);

	bool AddPlayerCharacterComponent(UAnimInstance* InAnimInstance); 

private:
	
	void OnRegisterLocalPlayer();
	
private:

	bool bPlayAutomatcallyOnAnimInstance;

	TObjectPtr<UKinetixComponent> LocalKinetixComponent;
	
	TMap<FAnimationID, TList<TDelegate<void()>>> CallbacksOnRetargetedAnimationIDOnLocalPlayer;
	TList<FAnimationID>* EmotesToPreload;
	TList<FAnimationID>* DownloadedEmotesReadyToPlay;
	
};
