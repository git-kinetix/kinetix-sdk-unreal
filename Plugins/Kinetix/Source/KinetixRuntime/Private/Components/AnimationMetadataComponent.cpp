// // Copyright Kinetix. All Rights Reserved.


#include "Components/AnimationMetadataComponent.h"

#include "Interfaces/IPluginManager.h"

#include "KinetixRuntimeModule.h"

// Sets default values for this component's properties
UAnimationMetadataComponent::UAnimationMetadataComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAnimationMetadataComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	UE_LOG(LogKinetixRuntime, Warning, TEXT("Module name: %s"), *FKinetixRuntimeModule::GetName());
	TSharedPtr<IPlugin> KinetixPlugin = IPluginManager::Get().GetModuleOwnerPlugin(*FKinetixRuntimeModule::GetName());
	if (KinetixPlugin.IsValid())
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("%s"), *KinetixPlugin->GetName());
	}
}
