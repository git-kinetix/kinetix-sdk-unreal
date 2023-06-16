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

	const TArray<FKinetixEmote> FetchMetadatas();

	void AddEmoteFromMetadata(const FAnimationMetadata& InAnimationMetadata);

	void AddEmoteFromID(const FAnimationID& InAnimationID);

	bool HasEmote(const FAnimationID& InAnimationID);

	FString GetAccountID() const { return AccountID; }

	void TestFunc();
	void MetadataRequestComplete(
		TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
		TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
		bool bSuccess);

private:
	const FString AccountID;

	TSet<FKinetixEmote> Emotes;
};
