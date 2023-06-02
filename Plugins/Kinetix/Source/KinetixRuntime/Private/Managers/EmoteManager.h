// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Emote/KinetixEmote.h"

class FKinetixEmote;

/**
 * 
 */
class FEmoteManager
{
public:
	FEmoteManager();
	~FEmoteManager();

	FKinetixEmote* GetEmote(const FAnimationID& InAnimationID);
	TArray<FKinetixEmote*> GetEmotes(TArray<FAnimationID>& InAnimationIDs);
	TArray<FKinetixEmote*> GetAllEmotes();

	void GetAnimSequence(const FAnimationID& InAnimationID, const TDelegate<void(UAnimSequence*)>& OnSuccess);
	void AnimSequenceAvailable(FSoftObjectPath SoftObjectPath, TDelegate<void(UAnimSequence*)> Delegate) const;
	
private:

	TMap<FAnimationID, FKinetixEmote> KinetixEmotes;
};
