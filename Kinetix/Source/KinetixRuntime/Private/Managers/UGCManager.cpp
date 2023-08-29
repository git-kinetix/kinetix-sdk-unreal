// Copyright Kinetix. All Rights Reserved.

#include "Managers/UGCManager.h"

// Otherwise there is no exports of static symbols
TUniquePtr<FUGCManager> FUGCManager::Instance(nullptr);

FUGCManager::FUGCManager()
{
}

FUGCManager::~FUGCManager()
{
	Instance = nullptr;
}

FUGCManager& FUGCManager::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeUnique<FUGCManager>();
	}
	return *Instance;
}
