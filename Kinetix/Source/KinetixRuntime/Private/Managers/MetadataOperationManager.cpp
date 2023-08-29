// Copyright Kinetix. All Rights Reserved.

#include "Managers/MetadataOperationManager.h"

#include "HttpModule.h"
#include "KinetixDeveloperSettings.h"
#include "Core/Account/KinetixAccount.h"
#include "Core/Metadata/KinetixMetadata.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

FMetadataOperationManager::FMetadataOperationManager()
{
}

FMetadataOperationManager::~FMetadataOperationManager()
{
}

void FMetadataOperationManager::GetAnimationMetadataOfEmote(const FAnimationMetadata& InAnimationID,
                                                            const TDelegate<void(
	                                                            FAnimationMetadata InAnimationMetadata)>& Callback)
{
	UE::Tasks::Launch(UE_SOURCE_LOCATION, [InAnimationID, Callback]()
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL(
			SDKAPIUrlBase + FString::Printf(
				SDKAPIEmoteUrl, *InAnimationID.Id.UUID.ToString(EGuidFormats::DigitsWithHyphensLower)));
		Request->SetVerb(TEXT("GET"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetHeader(TEXT("accept"), TEXT("application/json"));
		Request->SetHeader(
			TEXT("x-api-key"), GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration.VirtualWorld);

		Request->OnProcessRequestComplete().BindLambda([Callback](
			TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> OldRequest,
			TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
			bool bSuccess)
		{
			if (!Response.IsValid())
			{
				UE_LOG(LogKinetixMetadata, Warning,
					   TEXT("[FMetadataOperationManager] OnGetEmotesToVirtualWorldResponse: Failed to connect to service"));
				return;
			}

			if (Response->GetResponseCode() == EHttpResponseCodes::Denied)
			{
				UE_LOG(LogKinetixAccount, Warning,
					   TEXT(
						   "[FMetadataOperationManager] OnGetEmotesToVirtualWorldResponse: Emote is already registered or there is an error in the request : %s!"
					   ), *Response->GetContentAsString());
				return;
			}

			const FString JsonString = Response->GetContentAsString();
			FAnimationMetadata AnimationMetadata;
			if(!UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromJson(JsonString, AnimationMetadata))
				return;

			Callback.ExecuteIfBound(AnimationMetadata);
		});
		
		if (!Request->ProcessRequest())
			UE_LOG(LogKinetixMetadata, Warning, TEXT("[FMetadataOperationManager] GetAnimationMetadata(): Unable to process request !"));
	});
}
