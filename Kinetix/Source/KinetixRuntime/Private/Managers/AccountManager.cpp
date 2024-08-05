// Copyright Kinetix. All Rights Reserved.

#include "AccountManager.h"

#include "EmoteManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "KinetixDeveloperSettings.h"
#include "KinetixRuntimeModule.h"
#include "UGCManager.h"
#include "Core/Account/KinetixAccount.h"
#include "Interfaces/IHttpResponse.h"
#include "Tasks/Pipe.h"
#include "Tasks/Task.h"
#include "Data/KinetixDataLibrary.h"

// TUniquePtr<FAccountManager> FAccountManager::Instance = nullptr;
FAccountManager* FAccountManager::Instance = nullptr;

FAccountManager::FAccountManager()
	: LoggedAccount(nullptr), AssignEmotePipe(TEXT("AssignEmotePipe"))
{
	// check(!AssociateEmoteEvent.IsValid());
	// AssociateEmoteEvent = MakeUnique<UE::Tasks::FTaskEvent>(UE_SOURCE_LOCATION);
}

FAccountManager::~FAccountManager()
{
	// Instance = nullptr;

	AccountPoller.StopPolling();
}

void FAccountManager::SetVirtualWorldID(const FString& InVirtualWorldID)
{
	VirtualWorldID = InVirtualWorldID;
}

bool FAccountManager::ConnectAccount(const FString& InUserID)
{
	if (VirtualWorldID.IsEmpty())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] ConnectAccount: VirtualWorldID is null !"));
		return false;
	}

	UserID = InUserID;

	if (IsAccountConnected(UserID))
	{
		UE_LOG(LogKinetixAccount, Log, TEXT("[FAccountManager] ConnectAccount: %s already connected !"), *InUserID);
		return false;
	}

	if (!AccountExists(UserID))
	// && !TryCreateAccount(InUserID))
	{
		// From here it's just that we couldn't send the request
		return false;
	}

	return true;
}

void FAccountManager::FinishAccountConnection()
{
	if (LoggedAccount != nullptr)
	{
		UE_LOG(LogKinetixAccount, Log, TEXT("[FAccountManager] ConnectAccount: %s already connected !"),
		       *LoggedAccount->GetAccountID());
		DisconnectAccount();
	}

	LoggedAccount = new FAccount(UserID, false);
	UserID.Empty();

	Accounts.Add(LoggedAccount);

	OnUpdatedAccountDelegate.Broadcast();
	OnConnectedAccountDelegate.Broadcast();

	AccountPoller.StartPolling();
}

void FAccountManager::DisconnectAccount()
{
}

bool FAccountManager::AssociateEmotesToVirtualWorld(const TArray<FAnimationID>& InEmotes)
{
	if (VirtualWorldID.IsEmpty())
		return false;

	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FAccountManager::OnGetEmotesToVirtualWorldResponse);

	Request->SetURL(GetDefault<UKinetixDeveloperSettings>()->SDKAPIUrlBase + SDKAPIVirtualWorldEmoteUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("accept"), TEXT("application/json"));
	Request->SetHeader(TEXT("x-api-key"), VirtualWorldID);
	Request->SetHeader(TEXT("User-Agent"), SDKUSERAGENT);

	FString UUIDs;
	for (int i = 0; i < InEmotes.Num(); ++i)
	{
		UUIDs += FString::Printf(TEXT("\"%s\""),
		                         *InEmotes[i].UUID.ToString(EGuidFormats::DigitsWithHyphensLower));
		if (i != (InEmotes.Num() - 1))
		{
			UUIDs += ",";
		}
	}

	Request->SetContentAsString(FString::Printf(TEXT("{\"uuids\":[%s]}"), *UUIDs));

	UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] AssociateEmotesToVirtualWorld: %s"),
	       *FString::Printf(TEXT("{\"uuids\":%s}"), *UUIDs));
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogKinetixAccount, Warning,
		       TEXT("[FAccountManager] AssociateEmotesToVirtualWorld: %s Error in Http request"),
		       *LoggedAccount->GetAccountID());
		return false;
	}
	return true;
}

bool FAccountManager::IsAccountConnected(const FString& InUserID)
{
	return false;
}

void FAccountManager::GetAllUserAnimationMetadatas(
	const FOnMetadatasAvailable& OnMetadatasAvailable, TDelegate<void()> OnFailed)
{
	if (!LoggedAccount)
		return;
	LoggedAccount->FetchMetadatas();
	LoggedAccount->RegisterOrCallMetadatasAvailable(OnMetadatasAvailable);
}

void FAccountManager::IsAnimationOwnedByUser(const FAnimationID& InAnimationID, const TDelegate<bool()>& OnSuccess,
                                             const TDelegate<void()>& OnFailure)
{
}

void FAccountManager::GetUserAnimationMetadatasByPage(int InCount, int InPage,
                                                      TDelegate<void(const TArray<FAnimationMetadata>&)>&
                                                      OnMetadatasAvailable, const TDelegate<void()>& OnFailure)
{
}

FAccount* FAccountManager::GetConnectedAccount() const
{
	return LoggedAccount;
}

bool FAccountManager::TryCreateAccount(const FString& InUserID)
{
	FHttpModule& HttpModule = FHttpModule::Get();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FAccountManager::OnGetHttpResponse);

	Request->SetURL(GetDefault<UKinetixDeveloperSettings>()->SDKAPIUrlBase + SDKAPIUsersUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("accept"), TEXT("application/json"));
	Request->SetHeader(TEXT("x-api-key"), VirtualWorldID);
	Request->SetHeader(TEXT("User-Agent"), SDKUSERAGENT);

	Request->SetContentAsString(FString::Printf(TEXT("{\"id\":\"%s\"}"), *InUserID));

	UE_LOG(LogKinetixAccount, Log, TEXT("%s"), *Request->GetContentType());
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] TryCreateAccount: %s Error in Http request"),
		       *InUserID);
		return false;
	}

	UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] TryCreateAccount: %s"), *InUserID);
	return false;
}

FAccountManager* FAccountManager::Get()
{
	if (!Instance)
	{
		Instance = new FAccountManager();
	}
	return Instance;
}

void FAccountManager::StopPolling()
{
	AccountPoller.StopPolling();
}

bool FAccountManager::AccountExists(const FString& InUserID)
{
	FHttpModule& HttpModule = FHttpModule::Get();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FAccountManager::OnGetUserResponse);

	UE_LOG(LogKinetixRuntime, Log, TEXT("SDKAPIUrlBase %s"),
		*GetDefault<UKinetixDeveloperSettings>()->SDKAPIUrlBase);
	Request->SetURL(GetDefault<UKinetixDeveloperSettings>()->SDKAPIUrlBase + SDKAPIUsersUrl + "/" + InUserID);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("accept"), TEXT("application/json"));
	Request->SetHeader(TEXT("x-api-key"), VirtualWorldID);
	Request->SetHeader(TEXT("User-Agent"), SDKUSERAGENT);

	UE_LOG(LogKinetixAccount, Log, TEXT("%s"), *Request->GetContentType());
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] AccountExists: %s Error in Http request"),
		       *InUserID);
		return false;
	}

	return true;
}

void FAccountManager::HandleUserExistsResponse(FString InUserID, bool bInResult)
{
	if (!bInResult)
	{
		TryCreateAccount(InUserID);
		return;
	}

	if (!OnConnectedAccountDelegate.IsBound())
		return;

	OnConnectedAccountDelegate.Broadcast();
}

void FAccountManager::OnGetHttpResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
                                        TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
                                        bool bConnectedSuccessfully)
{
	TSharedPtr<FJsonObject> JsonObject;
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] OnGetHttpResponse: Failed to connect to service"));
		return;
	}

	const FString RepsonseBody = HttpResponse->GetContentAsString();
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(RepsonseBody);

	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] OnGetHttpResponse: Failed to deserialize Json %s"),
		       *JsonReader->GetErrorMessage());
		return;
	}

	const int32 ResponseCode = HttpResponse->GetResponseCode();
	if (ResponseCode != 200)
	{
		FString ErrorLog = "ResponseCode : " + FString::FromInt(ResponseCode) + LINE_TERMINATOR;
		for (TTuple<FString, TSharedPtr<FJsonValue, ESPMode::ThreadSafe>> Value : JsonObject->Values)
		{
			ErrorLog += Value.Key + " : " + Value.Value->AsString() + LINE_TERMINATOR;
		}

		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] OnGetHttpResponse: Failed to connect to service %s"),
		       *ErrorLog);
		return;
	}

	FinishAccountConnection();
}

void FAccountManager::OnGetUserResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
                                        TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
                                        bool bConnectedSuccessfully)
{
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] OnGetUserResponse: Failed to connect to service"));
		return;
	}

	UE_LOG(LogKinetixAccount, Log, TEXT("[FAccountManager] OnGetUserResponse: %i"), HttpResponse->GetResponseCode());

	if (HttpResponse->GetResponseCode() != 200)
	{
		TryCreateAccount(UserID);
		return;
	}

	FinishAccountConnection();
}

void FAccountManager::OnGetEmotesToVirtualWorldResponse(
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
	bool bSuccess)
{
	if (!Response.IsValid())
	{
		UE_LOG(LogKinetixAccount, Warning,
		       TEXT("[FAccountManager] OnGetEmotesToVirtualWorldResponse: Failed to connect to service"));
		return;
	}

	if (Response->GetResponseCode() == EHttpResponseCodes::Denied)
	{
		UE_LOG(LogKinetixAccount, Warning,
		       TEXT(
			       "[FAccountManager] OnGetEmotesToVirtualWorldResponse: Emote is already registered or there is an error in the request : %s!"
		       ), *Response->GetContentAsString());
		return;
	}

	// check(AssociateEmoteEvent.IsValid());
	// AssociateEmoteEvent->Trigger();
	//AssociateEmoteEvent.Reset();
}
