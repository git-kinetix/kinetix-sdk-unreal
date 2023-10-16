// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Templates/UniquePtr.h"

/**
 * 
 */
class KINETIXRUNTIME_API FUGCManager
{
public:
	
	FUGCManager();
	~FUGCManager();

	void StartPollingUGC();

	void StartPollingForNewUGCToken();

	void GetUgcUrl(const TDelegate<void(FString)>& InUrlFetchedCallback);

	bool IsUGCAvailable();

	void OnPollingResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bSuccess);

	void OnUGCUrlResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bSuccess);

	void OnUGCTokenResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bSuccess);
	
	static FUGCManager& Get()
	{
		if (!Instance.IsValid())
		{
			Instance = MakeUnique<FUGCManager>();
		}
		return *Instance;
	}

private:
	
	static TUniquePtr<FUGCManager> Instance;

	bool bEnableUGC;

	FString UgcUrl;
	FString TokenUUID;

	int FetchTimeOutInMinutes;
	FDateTime LastFetchedDate;
	
	TDelegate<void(FString)> UgCUrlFetchedCallback;
};