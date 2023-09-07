// Copyright Kinetix. All Rights Reserved.

#include "Managers/UGCManager.h"

#include "AccountManager.h"
#include "EmoteManager.h"
#include "HttpModule.h"

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

	TSet<FKinetixEmote*> Emotes = ConnectedAccount->GetEmotes();

	FString Url = SDKAPIUrlBase + FString::Printf(SDKAPIEmoteUsersUrl, *ConnectedAccount->GetAccountID());

	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();

	// Request->OnProcessRequestComplete().BindSP(
	// 	this, &FUGCManager::OnPollingResponse);
	//
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

void FUGCManager::OnPollingResponse(TSharedPtr<IHttpRequest> Shared, TSharedPtr<IHttpResponse> SharedPtr, bool bArg)
{
}
