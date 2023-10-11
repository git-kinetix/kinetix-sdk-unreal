// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Emote/KinetixEmote.h"
#include "Interfaces/IHttpRequest.h"
#include "Tasks/Task.h"

/**
 * 
 */
class FAccount
{
public:
	
	FAccount(const FString& InUserID, bool bPreFetch = true);
	~FAccount();

	const TArray<FKinetixEmote*> FetchMetadatas();

	void AddEmoteFromMetadata(const FAnimationMetadata& InAnimationMetadata);

	void AddEmoteFromID(const FAnimationID& InAnimationID);

	bool HasEmote(const FAnimationID& InAnimationID);

	FString GetAccountID() const { return AccountID; }

	void MetadataRequestComplete(
		TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
		TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
		bool bSuccess);

	void RegisterOrCallMetadatasAvailable(
		const FOnMetadatasAvailable& OnMetadatasAvailable);

	const TSet<FKinetixEmote*> GetEmotes() const;
	
protected:
	
	void AnimationRequestComplete(
		TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
		TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
		bool bSuccess, FAnimationMetadata InAnimationMetadata, FKinetixEmote* InConcernedEmote);

	void CallMetadatasAvailableDelegates();

private:
	const FString AccountID;

	TSet<FKinetixEmote*> Emotes;
	TSet<FAnimationMetadata> Metadatas;
	
	TArray<FOnMetadatasAvailable> OnMetadatasAvailableDelegates;
	
	bool bPendingRequest;
};
