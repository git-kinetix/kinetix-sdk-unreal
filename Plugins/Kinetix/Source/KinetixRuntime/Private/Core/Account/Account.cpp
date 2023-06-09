// Copyright Kinetix. All Rights Reserved.


#include "Core/Account/Account.h"

Account::Account(const FString& InUserID)
	: AccountID(InUserID)
{
}

Account::~Account()
{
}

const TArray<FKinetixEmote> Account::FetchMetadatas()
{
	if (!Emotes.IsEmpty())
		return Emotes.Array();

	return Emotes.Array();
}

void Account::AddEmoteFromMetadata(const FAnimationMetadata& InAnimationMetadata)
{
}

void Account::AddEmoteFromID(const FAnimationID& InAnimationID)
{
}

bool Account::HasEmote(const FAnimationID& InAnimationID)
{
	return false;
}
