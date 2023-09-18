// // Copyright Kinetix. All Rights Reserved.


#include "KinetixDeveloperSettings.h"

UKinetixDeveloperSettings::UKinetixDeveloperSettings()
	: bLoadAtStartup(false)
{
	SectionName = TEXT("Kinetix Section");
}

void UKinetixDeveloperSettings::GetCoreConfiguration(FKinetixCoreConfiguration& OutCoreConfiguration)
{
	OutCoreConfiguration = GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration;
}
