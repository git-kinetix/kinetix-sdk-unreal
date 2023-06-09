// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Emote/KinetixEmote.h"

/**
 * 
 */
class Account
{
public:

	Account(const FString& InUserID);
	~Account();

	const TArray<FKinetixEmote> FetchMetadatas();

	void AddEmoteFromMetadata(const FAnimationMetadata& InAnimationMetadata);

	void AddEmoteFromID(const FAnimationID& InAnimationID);

	bool HasEmote(const FAnimationID& InAnimationID);
	
private:

	const FString AccountID;

	TSet<FKinetixEmote> Emotes;
	
};
