// Copyright Kinetix. All Rights Reserved.


#include "Core/Account/Account.h"

#include "HttpModule.h"
#include "KinetixDeveloperSettings.h"
#include "Core/Account/KinetixAccount.h"
#include "Interfaces/IHttpResponse.h"
#include "Tasks/Task.h"

FAccount::FAccount(const FString& InUserID, bool bPreFetch)
	: AccountID(InUserID)
{
	if (bPreFetch)
		FetchMetadatas();
}

FAccount::~FAccount()
{
}

const TArray<FKinetixEmote> FAccount::FetchMetadatas()
{
	if (!Emotes.IsEmpty())
		return Emotes.Array();

	// auto AnimationsMetadatas = ;
	UE::Tasks::FTask FetchTask = UE::Tasks::Launch(
		UE_SOURCE_LOCATION, [&]()
		{
			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
			Request->OnProcessRequestComplete().BindRaw(this, &FAccount::MetadataRequestComplete);

			Request->SetURL(SDKAPIUrlBase + FString::Printf(SDKAPIEmoteUsersUrl, *AccountID));
			Request->SetVerb(TEXT("GET"));
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
			Request->SetHeader(TEXT("accept"), TEXT("application/json"));
			Request->SetHeader(
				TEXT("x-api-key"), GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration.VirtualWorld);

			if (!Request->ProcessRequest())
				UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccount] FetchMetadatas(): Unable to process request !"));
		});

	return Emotes.Array();
}

void FAccount::AddEmoteFromMetadata(const FAnimationMetadata& InAnimationMetadata)
{
}

void FAccount::AddEmoteFromID(const FAnimationID& InAnimationID)
{
}

bool FAccount::HasEmote(const FAnimationID& InAnimationID)
{
	return false;
}

void FAccount::TestFunc()
{
	UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccount] TestFunc: On instance"))
}

void FAccount::MetadataRequestComplete(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
                                       TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response, bool bSuccess)
{
	if (!Response.IsValid())
	{
		UE_LOG(LogKinetixAccount, Warning,
		       TEXT("[FAccount] MetadataRequestComplete(): Failed to connect to service !"));
		return;
	}

	if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		UE_LOG(LogKinetixAccount, Warning,
		       TEXT("[FAccount] MetadataRequestComplete(): Wrong response from server %i !"),
		       Response->GetResponseCode());
		return;
	}

	const FString JsonResponse = Response->GetContentAsString();
	UE_LOG(LogKinetixAccount, Log, TEXT("%s"), *JsonResponse);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	const bool bDeserializationResult = FJsonSerializer::Deserialize(JsonReader, JsonArray);
	if (!bDeserializationResult)
	{
		UE_LOG(LogKinetixAccount, Warning,
		       TEXT("[FAccount] MetadataRequestComplete(): Unable to deserialize response ! %s"),
		       *JsonReader.Get().GetErrorMessage());
		return;
	}

	FAnimationMetadata AnimationMetadata;
	for (int i = 0; i < JsonArray.Num(); ++i)
	{
		const TSharedPtr<FJsonObject> ResponseObject = JsonArray[i]->AsObject();
		if (!ResponseObject.IsValid())
			continue;

		// UUID field
		FString EmoteUUID;
		ResponseObject->TryGetStringField(TEXT("emoteUuid"), EmoteUUID);
		FGuid::Parse(EmoteUUID, AnimationMetadata.Id.UUID);

		const TSharedPtr<FJsonObject>* DataObject;
		ResponseObject->TryGetObjectField(TEXT("data"), DataObject);
		if (!(DataObject && DataObject->IsValid()))
			continue;

		// Name field
		FString StringField;
		(*DataObject).Get()->TryGetStringField(TEXT("name"), StringField);
		AnimationMetadata.Name = FName(StringField);

		const TSharedPtr<FJsonObject>* MetadataObject = nullptr;
		(*DataObject)->TryGetObjectField(TEXT("metadata"), MetadataObject);
		if (!(MetadataObject && MetadataObject->IsValid()))
			continue;

		(*MetadataObject)->TryGetNumberField(TEXT("duration"), AnimationMetadata.Duration);
		(*MetadataObject)->TryGetStringField(TEXT("description"), StringField);
		AnimationMetadata.Description = FText::FromString(StringField);

		const TArray<TSharedPtr<FJsonValue>>* FilesObject;
		(*DataObject)->TryGetArrayField(TEXT("files"), FilesObject);
		if (!(FilesObject && FilesObject->Num()))
			continue;

		for (int j = 0; j < FilesObject->Num(); ++j)
		{
			TSharedPtr<FJsonObject> FileObject = (*FilesObject)[j]->AsObject();
			if (!FileObject.IsValid())
				continue;

			FileObject->TryGetStringField(TEXT("name"), StringField);
			if (StringField == TEXT("thumbnail"))
				FileObject->TryGetStringField(TEXT("url"), AnimationMetadata.IconURL.Map);
			else if (StringField == TEXT("glb"))
				FileObject->TryGetStringField(TEXT("url"), AnimationMetadata.AnimationURL.Map);
		}

		AddEmoteFromMetadata(AnimationMetadata);
	}

	// if (!UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromJson(JsonObjects, AnimationMetadata))
	// 	return;
}
