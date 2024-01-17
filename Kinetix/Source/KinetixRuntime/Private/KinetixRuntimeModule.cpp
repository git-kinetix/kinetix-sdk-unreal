// Copyright Epic Games, Inc. All Rights Reserved.

#include "KinetixRuntimeModule.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Data/KinetixDataLibrary.h"
#include "Managers/MemoryManager.h"

// #include "D:/Kinetix/KinanimLibrary/CustomHalf.h"

DEFINE_LOG_CATEGORY(LogKinetixRuntime);

#define LOCTEXT_NAMESPACE "FKinetixRuntimeModule"

__SumOf SumOfFunction;
__RegisterForLogs RegisterForLogsFunction;
__From FromFunction;
__Func VirtualFuncFunction;

void FKinetixRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
		FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	if (AssetRegistry.IsLoadingAssets())
	{
		AssetRegistry.OnFilesLoaded().AddRaw(this, &FKinetixRuntimeModule::OnAssetRegistryInitialized);
	}
	else
	{
		OnAssetRegistryInitialized();
	}

	// Loading Kinanim DLL
	const FString BasePluginDir = GetPlugin().Get()->GetBaseDir();

	const FString KinanimLibPath = FPaths::Combine(*BasePluginDir, TEXT("Binaries/ThirdParty/Kinetix/Kinanim.dll"));

	KinanimLibHandle = FPlatformProcess::GetDllHandle(*KinanimLibPath);

	if (KinanimLibHandle != nullptr)
	{
		// UE_LOG(LogTemp, Log, TEXT("Kinanim.dll loaded successfully!"));
		//
		// SumOfFunction = nullptr;
		// FString ProcName = "SumOf";
		// SumOfFunction = (__SumOf)FPlatformProcess::GetDllExport(
		// 	KinanimLibHandle, *ProcName);
		// if (SumOfFunction != nullptr)
		// {
		// 	UE_LOG(LogTemp, Log, TEXT("Kinanim.dll SumOf function loaded successfully!"));
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("Kinanim.dll SumOf function failed to load!"));
		// }
		//
		// RegisterForLogsFunction = nullptr;
		// ProcName = "RegisterForLogs";
		// RegisterForLogsFunction = (__RegisterForLogs)FPlatformProcess::GetDllExport(
		// 	KinanimLibHandle, *ProcName);
		// if (RegisterForLogsFunction != nullptr)
		// {
		// 	UE_LOG(LogTemp, Log, TEXT("Kinanim.dll SumOf function loaded successfully!"));
		//
		// 	RegisterForLogsFunction([=](int RawLogType, const char* RawLog)
		// 	{
		// 		FString LogString = RawLog;
		// 		ELogType KinanimLogType = (ELogType)RawLogType;
		//
		// 		switch (KinanimLogType)
		// 		{
		// 		case ELogType::LT_Log:
		// 			UE_LOG(LogTemp, Log, TEXT("%s"), *LogString);
		// 			break;
		// 		case ELogType::LT_Warning:
		// 			UE_LOG(LogTemp, Warning, TEXT("%s"), *LogString);
		// 			break;
		// 		case ELogType::LT_Error:
		// 			UE_LOG(LogTemp, Error, TEXT("%s"), *LogString);
		// 			break;
		// 		}
		// 	});
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("Kinanim.dll RegisterForLogs function failed to load!"));
		// }
		//
		// FromFunction = nullptr;
		// ProcName = "From";
		// FromFunction = (__From)FPlatformProcess::GetDllExport(KinanimLibHandle, *ProcName);
		// if (FromFunction)
		// {
		// 	UE_LOG(LogTemp, Log, TEXT("Kinanim.dll From function loaded successfully!"));
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("Kinanim.dll From function failed to load!"));
		// }
		//
		// VirtualFuncFunction = nullptr;
		// ProcName = TEXT("Func");
		// VirtualFuncFunction = (__Func)FPlatformProcess::GetDllExport(KinanimLibHandle, *ProcName);
		// if (VirtualFuncFunction)
		// {
		// 	UE_LOG(LogTemp, Log, TEXT("Kinanim.dll Func function loaded successfully!"));
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("Kinanim.dll Func function failed to load!"));
		// }
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Kinanim.dll failed to load!"));
	}

	if (SumOfFunction != nullptr)
	{
		int a = 42;
		int b = 24;
		int Result = SumOfFunction(a, b);

		if (Result == (a + b))
		{
			UE_LOG(LogTemp, Log, TEXT("Kinanim.dll SumOf function executed successfully!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Kinanim.dll SumOf function executed but not working!"));
		}
	}

	short s = 42;
	// float Result = KinanimLibrary::CustomHalf::From(s);
	// if (FMath::IsNearlyEqual(Result, 42.f))
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("KinanimLibrary.lib: From function worked"))
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("KinanimLibrary.lib: From function failed!"))
	// }

	// KinanimLibrary::InstanciatedClass* Instance = new KinanimLibrary::InstanciatedClass();
	// float InstanceResult = Instance->Get42();
	// if (FMath::IsNearlyEqual(InstanceResult, 42.f))
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("KinanimLibrary.lib: Instance.Get42 function worked"))
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("KinanimLibrary.lib: Instance.Get42 function failed!"))
	// }
}

void FKinetixRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FKinetixRuntimeModule::OnAssetRegistryInitialized()
{
	// Initialize manager no matter what
	FMemoryManager::Get();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FKinetixRuntimeModule, KinetixRuntime)
