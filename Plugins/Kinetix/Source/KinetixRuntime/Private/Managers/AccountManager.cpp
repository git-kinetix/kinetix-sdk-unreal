// Copyright Kinetix. All Rights Reserved.

#include "AccountManager.h"

#include "HttpModule.h"
#include "Core/Account/KinetixAccount.h"
#include "Interfaces/IHttpResponse.h"

FAccountManager::FAccountManager(const FString& InVirtualWorld)
	: VirtualWorldID(InVirtualWorld)
{
}

FAccountManager::~FAccountManager()
{
}

void FAccountManager::FinishAccountConnection()
{
	if (!LoggedAccount.IsEmpty())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] ConnectAccount: %s already connected !"),
		       *LoggedAccount);
		DisconnectAccount();
	}

	LoggedAccount = UserID;
	UserID.Empty();

	// LoggedAccount.FetchMetadatas();
	Accounts.Add(LoggedAccount);

	OnUpdatedAccountDelegate.Broadcast();
	OnConnectedAccountDelegate.Broadcast();
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
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] ConnectAccount: %s already connected !"), *InUserID);
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

void FAccountManager::DisconnectAccount()
{
}

bool FAccountManager::AssociateEmotesToVirtualWorld(const TArray<FAnimationID>& InEmotes)
{
	return false;
}

bool FAccountManager::AssociateEmoteToUser(const FAnimationID& InEmote)
{
	return false;
}

bool FAccountManager::IsAccountConnected(const FString& InUserID)
{
	return false;
}

void FAccountManager::GetAllUserAnimationMetadatas(
	const TDelegate<void(TArray<FAnimationMetadata>&)>& OnMetadatasAvailable, TDelegate<void()> OnFailed)
{
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

bool FAccountManager::TryCreateAccount(const FString& InUserID)
{
	FHttpModule& HttpModule = FHttpModule::Get();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FAccountManager::OnGetHttpResponse);

	Request->SetURL(SDKAPIUrlBase + SDKAPIUsersUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("accept"), TEXT("application/json"));
	Request->SetHeader(TEXT("x-api-key"), VirtualWorldID);
	Request->SetContentAsString(FString::Printf(TEXT("{\"id\":\"%s\"}"), *InUserID));

	UE_LOG(LogKinetixAccount, Warning, TEXT("%s"), *Request->GetContentType());
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] TryCreateAccount: %s Error in Http request"),
		       *InUserID);
		return false;
	}

	UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] TryCreateAccount: %s"), *InUserID);
	return false;
}

bool FAccountManager::AccountExists(const FString& InUserID)
{
	FHttpModule& HttpModule = FHttpModule::Get();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FAccountManager::OnGetUserResponse);

	Request->SetURL(SDKAPIUrlBase + SDKAPIUsersUrl + "/" + InUserID);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("accept"), TEXT("application/json"));
	Request->SetHeader(TEXT("x-api-key"), VirtualWorldID);

	UE_LOG(LogKinetixAccount, Warning, TEXT("%s"), *Request->GetContentType());
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
}

void FAccountManager::OnGetUserResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
                                        TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse,
                                        bool bConnectedSuccessfully)
{
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] OnGetUserResponse: Failed to connect to service"),
		);
		return;
	}

	if (HttpResponse->GetResponseCode() != 200)
	{
		TryCreateAccount(UserID);
		return;
	}

	FinishAccountConnection();
}
