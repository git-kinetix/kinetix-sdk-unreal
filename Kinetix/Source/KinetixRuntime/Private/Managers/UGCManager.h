// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Templates/UniquePtr.h"

/**
 * 
 */
class FUGCManager
{
public:
	FUGCManager();
	~FUGCManager();

	void StartPollingUGC();

	void StartPollingForNewUGCToken();

	void GetUgcUrl(const TDelegate<void(FString)>& UrlFetchedCallback);

	bool IsUGCAvailable();

	void OnPollingResponse(TSharedPtr<IHttpRequest> Shared, TSharedPtr<IHttpResponse> SharedPtr, bool bArg);

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

};