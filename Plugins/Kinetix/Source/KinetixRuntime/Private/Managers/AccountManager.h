// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"

class IHttpRequest;
class IHttpResponse;
/**
 * 
 */
class FAccountManager
{
public:

	FAccountManager(const FString& InVirtualWorld);
	~FAccountManager();
	void FinishAccountConnection();

	bool ConnectAccount(const FString& InUserID);
	void DisconnectAccount();
	
	bool AssociateEmotesToVirtualWorld(const TArray<FAnimationID>& InEmotes);
	bool AssociateEmoteToUser(const FAnimationID& InEmote);

	bool IsAccountConnected(const FString& InUserID);

	void GetAllUserAnimationMetadatas(const TDelegate<void(TArray<FAnimationMetadata>&)>& OnMetadatasAvailable, TDelegate<void()> OnFailed);

	void IsAnimationOwnedByUser(const FAnimationID& InAnimationID, const TDelegate<bool()>& OnSuccess, const TDelegate<void()>& OnFailure);

	void GetUserAnimationMetadatasByPage(int InCount, int InPage, TDelegate<void(const TArray<FAnimationMetadata>&)>& OnMetadatasAvailable, const TDelegate<void()>& OnFailure);

	DECLARE_MULTICAST_DELEGATE(FOnUpdatedAccount);
	FOnUpdatedAccount& OnUpdatedAccount() { return OnUpdatedAccountDelegate; }
	
	DECLARE_MULTICAST_DELEGATE(FOnConnectedAccount);
	FOnConnectedAccount& OnConnectedAccount() { return OnConnectedAccountDelegate; }

private:
	bool AccountExists(const FString& InUserID);
	bool TryCreateAccount(const FString& InUserID);
	void HandleUserExistsResponse(FString InUserID, bool bInResult);

	void OnGetHttpResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bConnectedSuccessfully);
	void OnGetUserResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
		TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
		bool bConnectedSuccessfully);

private:
	FString VirtualWorldID;

	// Cache the currently trying to connect ID
	FString UserID;

	FString LoggedAccount;

	TSet<FString> Accounts;

	FOnUpdatedAccount OnUpdatedAccountDelegate;
	FOnConnectedAccount OnConnectedAccountDelegate;
};
