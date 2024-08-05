// Copyright Kinetix. All Rights Reserved.

#include "Tests/KinetixUnitTests.h"

#include "FileHelpers.h"
#include "KinetixDeveloperSettings.h"
#include "Core/KinetixCoreSubsystem.h"
#include "Tests/AutomationEditorCommon.h"

KinetixUnitTests::KinetixUnitTests()
{
}

KinetixUnitTests::~KinetixUnitTests()
{
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKinetixInitializeTest,
                                 "Kinetix.Core.Initialize",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FKinetixInitializeTest::RunTest(const FString& Parameters)
{
	const UKinetixDeveloperSettings* KinetixSettings = GetDefault<UKinetixDeveloperSettings>();
	TestNotNull(
		TEXT("Failed to load Kinetix Configuration for Kinetix.Core.Initialize test. Abort"),
		KinetixSettings);

	if(!TestTrue(TEXT("Failed to load Login map for Kinetix.Core.Initialize test. Abort"),
		FEditorFileUtils::LoadMap(TEXT("Login"), false, false)))
	{
		return false;
	}
		
	TestNotNull(TEXT("Failed to retreive the world for Kinetix.Core.Initialize, abort"), GEditor->PlayWorld.Get());
	if (!IsValid(GEditor->PlayWorld))
	{
		return false;
	}
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKinetixConnectTest,
                                 "Kinetix.Core.Core.Connect",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FKinetixConnectTest::RunTest(const FString& Parameters)
{
	return true;
}
