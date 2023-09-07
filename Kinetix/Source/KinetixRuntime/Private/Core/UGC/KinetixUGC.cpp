// Copyright Kinetix. All Rights Reserved.

#include "Core/UGC/KinetixUGC.h"

#include "Core/Account/KinetixAccount.h"

UKinetixUGC::UKinetixUGC()
{}

UKinetixUGC::UKinetixUGC(FVTableHelper& Helper)
{}

UKinetixUGC::~UKinetixUGC()
{}

void UKinetixUGC::Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult)
{
	IKinetixSubcoreInterface::Initialize_Implementation(CoreConfiguration, bResult);

	bResult = true;
}

bool UKinetixUGC::IsUGCAvailable() const
{
	return FUGCManager::Get().IsUGCAvailable();
}

void UKinetixUGC::StartPollingForUGC() const
{
	FUGCManager::Get().StartPollingUGC();
}

void UKinetixUGC::StartPollingForNewUGCToken() const
{
	FUGCManager::Get().StartPollingForNewUGCToken();
}

void UKinetixUGC::GetUGCUrl(const FOnUGCUrlFetched& InUrlFetchedCallback)
{
	UrlFetchedCallback = InUrlFetchedCallback;
	FUGCManager::Get().GetUgcUrl(
		TDelegate<void(FString)>::CreateUObject(this, &UKinetixUGC::OnUrlFetched));
}

void UKinetixUGC::OnUrlFetched(FString String)
{
	UrlFetchedCallback.ExecuteIfBound(String);
}
