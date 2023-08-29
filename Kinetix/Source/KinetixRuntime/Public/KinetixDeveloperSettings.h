// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "Engine/DeveloperSettings.h"
#include "KinetixDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName = "Kinetix Settings"))
class KINETIXRUNTIME_API UKinetixDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UKinetixDeveloperSettings();

	UFUNCTION(BlueprintPure, Category="Kinetix|Settings")
	static void GetCoreConfiguration(FKinetixCoreConfiguration& OutCoreConfiguration);

public:

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General", meta = (DisplayName="Load At Startup"))
	bool bLoadAtStartup;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General", meta = (DisplayName="Core Configuration"))
	FKinetixCoreConfiguration CoreConfiguration;
};
