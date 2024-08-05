// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeAsset.h"
#include "Emote/KinetixEmote.h"
#include "SmartCache/KinetixCacheSaveGame.h"
#include "Interfaces/IHttpRequest.h"
#include "KinanimWrapper/KinanimParser.h"
#include "Templates/UniquePtr.h"

class UKinanimBonesDataAsset;
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
	void SetCurveRemapper(const FglTFRuntimeAnimationCurveRemapper& InRemapper);

	void SetBoneMapping(const UKinanimBonesDataAsset* InKinanimBoneMapping);

	static FEmoteManager& Get();

	void SetReferenceSkeleton(USkeletalMesh* SkeletalMesh);
	USkeletalMesh* GetReferenceSkeleton() const;
	bool GetAnimSequenceFromGltfAsset(const FKinetixEmote* InEmote, UglTFRuntimeAsset* LoadedGltfAsset);

	void LoadBoneMapping(const FKinetixEmote* InEmote, const TDelegate<void()>& OnOperationFinished,
	                     FString AvatarUUID);
	
	void MappingRequestComplete(
		TSharedPtr<IHttpRequest> HttpRequest,
		TSharedPtr<IHttpResponse> HttpResponse,
		bool bSuccess, FAnimationMetadata AnimationMetadata, FString AvatarUUID, TDelegate<void()> Delegate);

	void AnimationRequestComplete(
		TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
		TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
		bool bSuccess,
		FAnimationMetadata InAnimationMetadata,
		TDelegate<void()> OnSuccessDelegate);

	void HeaderRequestComplete(
		TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
		TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
		bool bSuccess,
		FAnimationMetadata InAnimationMetadata,
		TDelegate<void()> OnSuccessDelegate);

	void OnKinanimAvailableToPlay(
		UKinanimDownloader* KinanimDownloader, TDelegate<void()> OnSuccessDelegate);
	
	void OnKinanimComplete(UKinanimDownloader* KinanimDownloader, TDelegate<void()> OnSuccessDelegate);

	void LoadAnimation(const FKinetixEmote* InEmote, const TDelegate<void()>& OnOperationFinished, FString AvatarUUID = FString());

	void AnimationHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& HeaderValue);

	void AnimationRequestProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);

protected:
	TDelegate<void(FAssetData)> RefSkeletonLoadedDelegate;

	FglTFRuntimeAnimationCurveRemapper CurveRemapper;

	UPROPERTY()
	const UKinanimBonesDataAsset* KinanimBoneMapping;

	void OnReferenceSkeletonAvailable(FAssetData AssetData);

private:
	static TUniquePtr<FEmoteManager> Instance;

	TMap<FAnimationID, FKinetixEmote> KinetixEmotes;

	USkeletalMesh* ReferenceSkeletalMesh;
};
