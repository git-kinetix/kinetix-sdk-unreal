// // Copyright Kinetix. All Rights Reserved.


#include "KinetixDeveloperSettings.h"

UKinetixDeveloperSettings::UKinetixDeveloperSettings()
	: bLoadAtStartup(false), bSendPose(true), bEnableBlendshapes(true)
{
	SectionName = TEXT("Kinetix Section");
	SDKAPIUrlBase = TEXT("https://sdk-api.kinetix.tech");
}

void UKinetixDeveloperSettings::GetCoreConfiguration(FKinetixCoreConfiguration& OutCoreConfiguration)
{
	OutCoreConfiguration = GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration;
}

void UKinetixDeveloperSettings::GetSDKAPIUrlBase(FString& OutSDKAPIUrlBase)
{
	OutSDKAPIUrlBase = GetDefault<UKinetixDeveloperSettings>()->SDKAPIUrlBase;
}

void UKinetixDeveloperSettings::GetBlendshapeFlags(bool& OutBlendshapesFlag)
{
	OutBlendshapesFlag = GetDefault<UKinetixDeveloperSettings>()->bEnableBlendshapes;
}

bool UKinetixDeveloperSettings::GetLogFlag()
{
	FKinetixCoreConfiguration OutCoreConfiguration = GetDefault<UKinetixDeveloperSettings>()->CoreConfiguration;
	return OutCoreConfiguration.bShowLogs;
}
