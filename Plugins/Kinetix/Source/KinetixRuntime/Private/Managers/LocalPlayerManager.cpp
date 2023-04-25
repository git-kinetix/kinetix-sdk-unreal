// // Copyright Kinetix. All Rights Reserved.

#include "Managers/LocalPlayerManager.h"

#include "KinetixRuntimeModule.h"

FLocalPlayerManager::FLocalPlayerManager(bool bInPlayAutomaticallyOnAnimator)
	: bPlayAutomatcallyOnAnimInstance(bInPlayAutomaticallyOnAnimator)
{
	UE_LOG(LogKinetixRuntime, Log, TEXT("LocalPlayerManager instanciated"));
}

FLocalPlayerManager::~FLocalPlayerManager()
{
	UE_LOG(LogKinetixRuntime, Log, TEXT("LocalPlayerManager destroyed"));
}
