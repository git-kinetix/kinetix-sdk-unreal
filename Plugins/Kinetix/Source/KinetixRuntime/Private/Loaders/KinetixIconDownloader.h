// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "Emote/KinetixEmote.h"
#include "Interfaces/IHttpRequest.h"
#include "KinetixIconDownloader.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UKinetixIconDownloader : public UObject
{
	GENERATED_BODY()

public:

	UKinetixIconDownloader();

	void DownloadIcon(FKinetixEmote* Emote);

	void InjectOnIconAvailableDelegate(const FOnIconAvailable& OnIconAvailable);

	void ProcessIconRequest(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bArg);
	
public:
	
	FOnIconAvailable OnIconAvailableDelegate;
	
	FOnIconFail OnIconFailDelegate;
	
};
