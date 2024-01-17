// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Account/Account.h"
#include "Core/Account/AccountPoller.h"
#include "Data/KinetixDataLibrary.h"
#include "Templates/UniquePtr.h"
#include "Tasks/Pipe.h"

class IHttpRequest;
class IHttpResponse;
/**
 * 
 */
class FAccountManager
{
public:
	FAccountManager();
	~FAccountManager();

	void SetVirtualWorldID(const FString& InVirtualWorldID);

	void FinishAccountConnection();

	bool ConnectAccount(const FString& InUserID);
	void DisconnectAccount();

	bool AssociateEmotesToVirtualWorld(const TArray<FAnimationID>& InEmotes);

	bool IsAccountConnected(const FString& InUserID);

	void GetAllUserAnimationMetadatas(const FOnMetadatasAvailable& OnMetadatasAvailable,
	                                  TDelegate<void()> OnFailed);

	void IsAnimationOwnedByUser(const FAnimationID& InAnimationID, const TDelegate<bool()>& OnSuccess,
	                            const TDelegate<void()>& OnFailure);

	void GetUserAnimationMetadatasByPage(int InCount, int InPage,
	                                     TDelegate<void(const TArray<FAnimationMetadata>&)>& OnMetadatasAvailable,
	                                     const TDelegate<void()>& OnFailure);

	FAccount* GetConnectedAccount() const;

	DECLARE_MULTICAST_DELEGATE(FOnUpdatedAccount);
	FOnUpdatedAccount& OnUpdatedAccount() { return OnUpdatedAccountDelegate; }

	DECLARE_MULTICAST_DELEGATE(FOnConnectedAccount);
	FOnConnectedAccount& OnConnectedAccount() { return OnConnectedAccountDelegate; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnAssociatedEmote, const FString&);
	FOnAssociatedEmote& OnAssociatedEmote() { return OnAssociatedEmoteDelegate; }

	static FAccountManager& Get();

	void StopPolling();
	
private:
	bool AccountExists(const FString& InUserID);
	bool TryCreateAccount(const FString& InUserID);
	void HandleUserExistsResponse(FString InUserID, bool bInResult);

	void OnGetHttpResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
	                       TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bConnectedSuccessfully);

	void OnGetUserResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
	                       TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
	                       bool bConnectedSuccessfully);

	void OnGetEmotesToVirtualWorldResponse(
		TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
		TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
		bool bSuccess);

private:
	
	static TUniquePtr<FAccountManager> Instance;

	FString VirtualWorldID;

	// Cache the currently trying to connect ID
	FString UserID;

	FAccount* LoggedAccount;

	TSet<FAccount*> Accounts;

	TArray<FAnimationID> Emotes;

	UE::Tasks::FPipe AssignEmotePipe;
	TUniquePtr<UE::Tasks::FTaskEvent> AssociateEmoteEvent;

	FOnUpdatedAccount OnUpdatedAccountDelegate;
	FOnConnectedAccount OnConnectedAccountDelegate;
	FOnAssociatedEmote OnAssociatedEmoteDelegate;

	FAccountPoller AccountPoller;
};
