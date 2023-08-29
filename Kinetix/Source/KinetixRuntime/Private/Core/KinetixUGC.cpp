// Copyright Kinetix. All Rights Reserved.


#include "Core/KinetixUGC.h"

void UKinetixUGC::Initialize_Implementation(const FKinetixCoreConfiguration& CoreConfiguration, bool& bResult)
{
	IKinetixSubcoreInterface::Initialize_Implementation(CoreConfiguration, bResult);

	bResult = true;
}

bool UKinetixUGC::IsUGCAvailable() const
{
	return false;
}

void UKinetixUGC::StartPollingForUGC() const
{
}

void UKinetixUGC::StartPollingForNewUGCToken() const
{
}

void UKinetixUGC::GetUGCUrl() const
{
}
