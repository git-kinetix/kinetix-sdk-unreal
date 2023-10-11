// Copyright Kinetix. All Rights Reserved.

#include "Core/Account/AccountPoller.h"

#include "HttpModule.h"
#include "KinetixDeveloperSettings.h"
#include "Interfaces/IHttpResponse.h"
#include "Core/Account/KinetixAccount.h"
#include "Data/KinetixDataLibrary.h"
#include "Managers/AccountManager.h"

static const float IntervalTimeInSeconds = 60.f;

FAccountPoller::FAccountPoller(): bPollingEnabled(false)
{
}

FAccountPoller::~FAccountPoller()
{
}

bool FAccountPoller::GetPollingState() const
{
	return bPollingEnabled;
}

void FAccountPoller::StartPolling()
{
	bPollingEnabled = true;

	PollingTask =
		UE::Tasks::Launch(UE_SOURCE_LOCATION, [this]
		{
			while (bPollingEnabled)
			{
				UE::Tasks::FTask Task =
					UE::Tasks::Launch(UE_SOURCE_LOCATION, []
					{
						UE_LOG(LogKinetixAccount, Warning,
						       TEXT("[FAccountPoller] StartPolling(): New Polling !"));

						UE::Tasks::FTaskEvent Blocker{UE_SOURCE_LOCATION};
						UE::Tasks::AddNested(Blocker);
						TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
						Request->OnProcessRequestComplete().BindLambda([&Blocker]
						(TSharedPtr<IHttpRequest> OldRequest,
						 TSharedPtr<IHttpResponse> Response,
						 bool bSuccess)
							{
								if (!Response.IsValid())
								{
									UE_LOG(LogKinetixAccount, Warning,
									       TEXT("[FAccountPoller] StartPolling(): Failed to connect to service !"));
									Blocker.Trigger();
									return;
								}

								if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
								{
									UE_LOG(LogKinetixAccount, Warning,
									       TEXT("[FAccountPoller] StartPolling(): Wrong response from server %i !"),
									       Response->GetResponseCode());
									Blocker.Trigger();
									return;
								}

								const FString JsonResponse = Response->GetContentAsString();

								TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(
									Response->GetContentAsString());
								TSharedPtr<FJsonValue> ResponseValue;

								const bool bDeserializationResult =
									FJsonSerializer::Deserialize(JsonReader, ResponseValue);
								if (!bDeserializationResult)
								{
									UE_LOG(LogKinetixAccount, Warning,
									       TEXT("[FAccountPoller] StartPolling(): Unable to deserialize response !"));
									Blocker.Trigger();
									return;
								}

								const TArray<TSharedPtr<FJsonValue>> Elements = ResponseValue->AsArray();
								FAccount* Account = FAccountManager::Get().GetConnectedAccount();
								if (Account == nullptr)
								{
									Blocker.Trigger();
									return;
								}

								UE_LOG(LogKinetixAccount, Warning,
								       TEXT("[FAccountPoller] StartPolling(): New Response !"));

								if (Account->GetEmotes().Num() >= Elements.Num())
								{
									Blocker.Trigger();
									return;
								}

								TSet<FKinetixEmote*> CurrentEmotes = Account->GetEmotes();
								FAnimationMetadata CurrentMetadata;
								bool bHasFoundMetadata = false;
								for (int i = 0; i < Elements.Num(); ++i)
								{
									TSharedPtr<FJsonObject> JsonObject = Elements[i].Get()->AsObject();
									if(!JsonObject.IsValid()) continue;

									const TSharedPtr<FJsonObject>* DataObject;
									JsonObject->TryGetObjectField(TEXT("data"), DataObject);
									
									UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromJson(
										*DataObject, CurrentMetadata);
										
									bHasFoundMetadata = false;

									for (FKinetixEmote* Emote : CurrentEmotes)
									{
										if (Emote->GetAnimationMetadata().Id.UUID == CurrentMetadata.Id.UUID)
										{
											bHasFoundMetadata = true;
											break;
										}
									}

									if (bHasFoundMetadata)
										continue;

									Account->AddEmoteFromMetadata(CurrentMetadata);
								}

								Blocker.Trigger();
							});

						Request->SetURL(SDKAPIUrlBase + FString::Printf(
							SDKAPIEmoteUsersUrl, *FAccountManager::Get().GetConnectedAccount()->GetAccountID()));
						Request->SetVerb(TEXT("GET"));
						Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
						Request->SetHeader(TEXT("accept"), TEXT("application/json"));
						Request->SetHeader(
							TEXT("x-api-key"), GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration.VirtualWorld);

						if (!Request->ProcessRequest())
							UE_LOG(LogKinetixAccount, Warning,
						       TEXT("[FAccountPoller] StartPolling(): Unable to process request !"));

						Blocker.BusyWait();
					});
				UE::Tasks::AddNested(Task);

				while (!Task.IsCompleted())
				{
					FPlatformProcess::Yield();
				}

				Task =
					UE::Tasks::Launch(UE_SOURCE_LOCATION, []
					{
						FPlatformProcess::Sleep(IntervalTimeInSeconds);
					});
				Task.BusyWait();
			}
		});
}

void FAccountPoller::StopPolling()
{
	bPollingEnabled = false;
}
