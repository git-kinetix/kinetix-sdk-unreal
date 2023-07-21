// Copyright Kinetix. All Rights Reserved.

#include "AccountManager.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Core/Account/KinetixAccount.h"
#include "Interfaces/IHttpResponse.h"
#include "Tasks/Pipe.h"
#include "Tasks/Task.h"

FAccountManager::FAccountManager(const FString& InVirtualWorld)
	: VirtualWorldID(InVirtualWorld), LoggedAccount(nullptr), AssignEmotePipe(TEXT("AssignEmotePipe"))
{
	check(!AssociateEmoteEvent.IsValid());
	AssociateEmoteEvent = MakeUnique<UE::Tasks::FTaskEvent>(UE_SOURCE_LOCATION);
}

FAccountManager::~FAccountManager()
{
	if (!AssociateEmoteEvent.IsValid())
		return;

	AssociateEmoteEvent->Trigger();
	AssociateEmoteEvent = nullptr;
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

void FAccountManager::FinishAccountConnection()
{
	if (LoggedAccount != nullptr)
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] ConnectAccount: %s already connected !"),
		       *LoggedAccount->GetAccountID());
		DisconnectAccount();
	}

	LoggedAccount = new FAccount(UserID, false);
	UserID.Empty();

	LoggedAccount->FetchMetadatas();

	Accounts.Add(LoggedAccount);

	OnUpdatedAccountDelegate.Broadcast();
	OnConnectedAccountDelegate.Broadcast();
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

	Request->SetURL(SDKAPIUrlBase + SDKAPIVirtualWorldEmoteUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("accept"), TEXT("application/json"));
	Request->SetHeader(TEXT("x-api-key"), VirtualWorldID);

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

bool FAccountManager::AssociateEmoteToUser(const FAnimationID& InEmote)
{
	if (LoggedAccount == nullptr)
	{
		UE_LOG(LogKinetixAccount, Warning, TEXT("[FAccountManager] AssociateEmoteToUser: No acount connected !"));
		return false;
	}

	if (LoggedAccount->HasEmote(InEmote))
		return true;

	Emotes.Empty();
	Emotes.Add(InEmote);

	AssignEmotePipe.Launch(UE_SOURCE_LOCATION, [&]
	{
		AssociateEmotesToVirtualWorld(Emotes);
	});

	AssignEmotePipe.Launch(UE_SOURCE_LOCATION, [&]
	{
		FHttpModule& HttpModule = FHttpModule::Get();
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();

		Request->OnProcessRequestComplete().BindLambda([&](
			TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> OldRequest,
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

				TSharedPtr<FJsonObject> JsonObject;
				if (!Response.IsValid())
				{
					UE_LOG(LogKinetixAccount, Warning,
					       TEXT("[FAccountManager] OnGetHttpResponse: Failed to connect to service"));
					return;
				}

				const FString RepsonseBody = Response->GetContentAsString();
				TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(RepsonseBody);

				if (!FJsonSerializer::Deserialize(JsonReader, JsonObject))
				{
					UE_LOG(LogKinetixAccount, Warning,
					       TEXT("[FAccountManager] OnGetHttpResponse: Failed to deserialize Json %s"),
					       *JsonReader->GetErrorMessage());
					return;
				}

				FString Message;
				JsonObject->TryGetStringField(TEXT("message"), Message);
				OnAssociatedEmoteDelegate.Broadcast(Message);
			
			});

		Request->SetURL(
			SDKAPIUrlBase +
			FString::Printf(
				SDKAPIEmoteUsersUrl,
				*LoggedAccount->GetAccountID()) + "/" + Emotes[0].UUID.ToString(EGuidFormats::DigitsWithHyphensLower)
		);

		UE_LOG(LogKinetixAccount, Log, TEXT("[FAccountManager] AssociateEmotesToUser: Generated URL %s"),
		       *Request->GetURL());

		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetHeader(TEXT("accept"), TEXT("application/json"));
		Request->SetHeader(TEXT("x-api-key"), VirtualWorldID);

		if (!Request->ProcessRequest())
		{
			UE_LOG(LogKinetixAccount, Warning,
			       TEXT("[FAccountManager] AssociateEmotesToUser: %s Error in Http request"),
			       *LoggedAccount->GetAccountID());
			return false;
		}

		return true;
	}, *AssociateEmoteEvent);

	return false;
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
	LoggedAccount->RegisterOrCallMetadatasAvailable(OnMetadatasAvailable);
	LoggedAccount->FetchMetadatas();
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

	check(AssociateEmoteEvent.IsValid());
	AssociateEmoteEvent->Trigger();
	//AssociateEmoteEvent.Reset();
}
