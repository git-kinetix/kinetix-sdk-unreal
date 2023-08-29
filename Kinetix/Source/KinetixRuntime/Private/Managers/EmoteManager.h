// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Emote/KinetixEmote.h"
#include "Templates/UniquePtr.h"

class IHttpResponse;
class IHttpRequest;
class FKinetixEmote;

/**
 * 
 */
class KINETIXRUNTIME_API FEmoteManager
{
public:
	FEmoteManager();
	~FEmoteManager();

	FKinetixEmote* GetEmote(const FAnimationID& InAnimationID);
	TArray<FKinetixEmote*> GetEmotes(TArray<FAnimationID>& InAnimationIDs);
	TArray<FKinetixEmote*> GetAllEmotes();

	void GetAnimSequence(const FAnimationID& InAnimationID, const TDelegate<void(UAnimSequence*)>& OnSuccess);
	void AnimSequenceAvailable(FSoftObjectPath SoftObjectPath, TDelegate<void(UAnimSequence*)> Delegate) const;

	static FEmoteManager& Get();

	USkeletalMesh* GetReferenceSkeleton() const;

	void AnimationRequestComplete(
		TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
		TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
		bool bSuccess,
		FAnimationMetadata InAnimationMetadata,
		TDelegate<void()> OnSuccessDelegate);
	
	void LoadAnimation(const FKinetixEmote* InEmote, const TDelegate<void()>& OnOperationFinished);

protected:
	
	TDelegate<void(FAssetData)> RefSkeletonLoadedDelegate;

	void OnReferenceSkeletonAvailable(FAssetData AssetData);

private:
	
	static TUniquePtr<FEmoteManager> Instance;

	TMap<FAnimationID, FKinetixEmote> KinetixEmotes;

	USkeletalMesh* ReferenceSkeletalMesh;
	
};
