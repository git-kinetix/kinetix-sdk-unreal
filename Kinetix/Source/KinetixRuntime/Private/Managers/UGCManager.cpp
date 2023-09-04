// Copyright Kinetix. All Rights Reserved.

#include "Managers/UGCManager.h"

#include "AccountManager.h"

TUniquePtr<FUGCManager> FUGCManager::Instance = nullptr;

FUGCManager::FUGCManager()
{
}

FUGCManager::~FUGCManager()
{
}

void FUGCManager::StartPollingUGC()
{
	FAccount* ConnectedAccount =
		FAccountManager::Get().GetConnectedAccount();

	if (ConnectedAccount == nullptr)
		return;

}

void FUGCManager::StartPollingForNewUGCToken()
{
}

void FUGCManager::GetUgcUrl(const TDelegate<void(FString)>& UrlFetchedCallback)
{
}

bool FUGCManager::IsUGCAvailable()
{
	return false;
}
