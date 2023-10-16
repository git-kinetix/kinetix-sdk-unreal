// Copyright Kinetix. All Rights Reserved.

#include "Managers/UGCManager.h"

#include "AccountManager.h"
#include "EmoteManager.h"
#include "HttpModule.h"
#include "KinetixDeveloperSettings.h"
#include "Core/UGC/KinetixUGC.h"
#include "Interfaces/IHttpResponse.h"

// Otherwise there is no exports of static symbols
TUniquePtr<FUGCManager> FUGCManager::Instance(nullptr);

FUGCManager::FUGCManager()
{
	bEnableUGC = true;
	FetchTimeOutInMinutes = 5;

	UE_LOG(LogKinetixUGC, Warning,
	       TEXT("[FUGCManager] HAS BEEN CONSTRUCTED!"));
	
}

FUGCManager::~FUGCManager()
{
	Instance = nullptr;
}

void FUGCManager::StartPollingUGC()
{
	FAccount* ConnectedAccount =
		FAccountManager::Get().GetConnectedAccount();

	if (ConnectedAccount == nullptr)
		return;

	TSet<FKinetixEmote*> Emotes = ConnectedAccount->GetEmotes();

	FString Url = SDKAPIUrlBase + FString::Printf(SDKAPIEmoteUsersUrl, *ConnectedAccount->GetAccountID());

	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();

	Request->OnProcessRequestComplete().BindRaw(
		this, &FUGCManager::OnPollingResponse);

	Request->SetURL(
		SDKAPIUrlBase + FString::Printf(SDKAPIEmoteUsersUrl, *ConnectedAccount->GetAccountID()));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("accept"), TEXT("application/json"));
	Request->SetHeader(
		TEXT("x-api-key"), GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration.VirtualWorld);

	if (!Request->ProcessRequest())
		UE_LOG(LogKinetixUGC,
	       Warning,
	       TEXT("[FUGCManager] StartPollingUGC(): Unable to process request !"));
}

void FUGCManager::StartPollingForNewUGCToken()
{
	if (TokenUUID.IsEmpty())
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] StartPollingForNewUGCToken(): Tried to poll UGC Token without calling GetUgcUrl!"));
		return;
	}

	FString Url = SDKAPIUrlBase + FString::Printf(KINETIXUGCTOKEN, *TokenUUID);

	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();

	Request->OnProcessRequestComplete().BindRaw(
		this, &FUGCManager::OnUGCTokenResponse);

	Request->SetURL(
		SDKAPIUrlBase + FString::Printf(KINETIXUGCTOKEN, *TokenUUID));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("accept"), TEXT("application/json"));
	Request->SetHeader(
		TEXT("x-api-key"),
		GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration.VirtualWorld);

	if (!Request->ProcessRequest())
		UE_LOG(LogKinetixUGC,
	       Warning,
	       TEXT("[FUGCManager] StartPollingUGC(): Unable to process request !"));
}

void FUGCManager::GetUgcUrl(const TDelegate<void(FString)>& InUrlFetchedCallback)
{
	if (UgcUrl.IsEmpty() || (FDateTime::Now() - LastFetchedDate).GetMinutes() > FetchTimeOutInMinutes)
	{
		const FAccount* ConnectedAccount = FAccountManager::Get().GetConnectedAccount();
		if (ConnectedAccount == nullptr)
			return;

		FString Uri = SDKAPIUrlBase + FString::Printf(KINETIXUGCURL, *ConnectedAccount->GetAccountID());

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->OnProcessRequestComplete().BindRaw(this, &FUGCManager::OnUGCUrlResponse);

		Request->SetURL(Uri);
		Request->SetVerb(TEXT("GET"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetHeader(TEXT("accept"), TEXT("application/json"));
		Request->SetHeader(
			TEXT("x-api-key"), GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration.VirtualWorld);

		UgCUrlFetchedCallback = InUrlFetchedCallback;

		if (!Request->ProcessRequest())
			UE_LOG(LogKinetixUGC, Warning, TEXT("[FUGCManager] GetUgcUrl(): Unable to process request !"));

		return;
	}

	InUrlFetchedCallback.ExecuteIfBound(UgcUrl);
}

bool FUGCManager::IsUGCAvailable()
{
	return bEnableUGC
		&& (FAccountManager::Get().GetConnectedAccount() != nullptr);
}

void FUGCManager::OnPollingResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bSuccess)
{
	if (!Response.IsValid())
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnPollingResponse(): Failed to connect to service !"));
		return;
	}

	if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnPollingResponse(): Wrong response from server %i !"),
		       Response->GetResponseCode());
		return;
	}

	const FString JsonResponse = Response->GetContentAsString();

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	TSharedPtr<FJsonValue> ResponseValue;

	const bool bDeserializationResult =
		FJsonSerializer::Deserialize(JsonReader, ResponseValue);
	if (!bDeserializationResult)
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnPollingResponse(): Unable to deserialize response !"));
		return;
	}

	const TArray<TSharedPtr<FJsonValue>> Elements = ResponseValue->AsArray();
	FAccount* Account = FAccountManager::Get().GetConnectedAccount();
	if (Account == nullptr)
		return;

	if (Account->GetEmotes().Num() >= Elements.Num())
		return;

	FAnimationMetadata AnimationMetadata;
	UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromJson(
		Elements[Elements.Num() - 1].Get()->AsObject(), AnimationMetadata);

	Account->AddEmoteFromMetadata(AnimationMetadata);

	UE_LOG(LogKinetixUGC, Warning,
	       TEXT("[FUGCManager] OnPollingResponse(): Unable to deserialize response !"));
}

void FUGCManager::OnUGCUrlResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response,
                                   bool bSuccess)
{
	if (!Response.IsValid())
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnUGCUrlResponse(): Failed to connect to service !"));
		return;
	}

	if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnUGCUrlResponse(): Wrong response from server %i !"),
		       Response->GetResponseCode());
		return;
	}

	const FString JsonResponse = Response->GetContentAsString();

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	TSharedPtr<FJsonValue> ResponseValue;

	const bool bDeserializationResult =
		FJsonSerializer::Deserialize(JsonReader, ResponseValue);
	if (!bDeserializationResult)
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnUGCUrlResponse(): Failed to deserialize response !"));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject = ResponseValue->AsObject();
	JsonObject.Get()->TryGetStringField(TEXT("uuid"), TokenUUID);
	JsonObject.Get()->TryGetStringField(TEXT("url"), UgcUrl);

	UgCUrlFetchedCallback.ExecuteIfBound(UgcUrl);
}

void FUGCManager::OnUGCTokenResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response,
                                     bool bSuccess)
{
	if (!Response.IsValid())
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnUGCTokenResponse(): Failed to connect to service !"));
		return;
	}

	if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnUGCTokenResponse(): Wrong response from server %i !"),
		       Response->GetResponseCode());
		return;
	}

	const FString JsonResponse = Response->GetContentAsString();

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	TSharedPtr<FJsonValue> ResponseValue;

	const bool bDeserializationResult =
		FJsonSerializer::Deserialize(JsonReader, ResponseValue);
	if (!bDeserializationResult)
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnUGCUrlResponse(): Failed to deserialize response !"));
		return;
	}

	FString StringValue;
	int NumValue = 0;
	TSharedPtr<FJsonObject> JsonObject = ResponseValue->AsObject();
	if (!JsonObject.Get()->TryGetNumberField(TEXT("expireIn"), NumValue))
	{
		UE_LOG(LogKinetixUGC, Warning,
		       TEXT("[FUGCManager] OnUGCUrlResponse(): Failed to deserialize response !"));
		return;
	}

	if (NumValue < 1)
	{
		TokenUUID.Empty();
		UgcUrl.Empty();
	}
}
