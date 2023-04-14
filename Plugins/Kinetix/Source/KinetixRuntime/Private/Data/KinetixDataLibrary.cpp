// // Copyright Kinetix. All Rights Reserved.


#include "Data/KinetixDataLibrary.h"

#include "JsonObjectWrapper.h"
#include "JsonBlueprintFunctionLibrary.h"
#include "KinetixRuntimeModule.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"

bool UKinetixDataBlueprintFunctionLibrary::GetJsonObjectFromFile(
	UObject* WorldContextObject, FString File, FJsonObjectWrapper& OutJsonObjectWrapper)
{
	FFilePath JsonFilePath;
	JsonFilePath.FilePath = File;
	return UJsonBlueprintFunctionLibrary::FromFile(WorldContextObject, JsonFilePath, OutJsonObjectWrapper);
}

bool UKinetixDataBlueprintFunctionLibrary::GetUUIDFromJson(FGuid& OutUUID,
                                                           const FJsonObjectWrapper& InJsonObjectWrapper)
{
	const TSharedRef<FJsonValue> UUIDJsonField =
		InJsonObjectWrapper.JsonObject->GetField<EJson::String>(TEXT("uuid")).ToSharedRef();
	FString UUIDValue;
	UUIDJsonField->TryGetString(UUIDValue);
	return FGuid::Parse(UUIDValue, OutUUID);
}

bool UKinetixDataBlueprintFunctionLibrary::GetNameFromJson(FName& OutName,
                                                           const FJsonObjectWrapper& InJsonObjectWrapper)
{
	bool Result = false;
	const TSharedRef<FJsonValue> NameJsonField =
		InJsonObjectWrapper.JsonObject->GetField<EJson::String>(TEXT("name")).ToSharedRef();
	FString TempValue;
	Result = NameJsonField->TryGetString(TempValue);
	OutName = FName(TempValue);
	return Result;
}

bool UKinetixDataBlueprintFunctionLibrary::GetOwnershipFromJson(EOwnership& OutOwnership,
                                                                const FJsonObjectWrapper& InJsonObjectWrapper)
{
	bool Result = false;
	const TSharedRef<FJsonValue> OwnershipJsonField =
		InJsonObjectWrapper.JsonObject->GetField<EJson::String>(TEXT("origin")).ToSharedRef();
	FString OwnershipValue;
	OwnershipJsonField->TryGetString(OwnershipValue);
	OwnershipValue = FString::Printf(TEXT("o_%s"), *OwnershipValue);
	OutOwnership = static_cast<EOwnership>(StaticEnum<EOwnership>()->GetValueByNameString(OwnershipValue));
	return true;
}

bool UKinetixDataBlueprintFunctionLibrary::GetDurationFromJson(float& OutDuration,
                                                               const FJsonObjectWrapper& InJsonObjectWrapper)
{
	const TSharedRef<FJsonValue> DurationJsonField =
		InJsonObjectWrapper.JsonObject->GetField<EJson::Number>(TEXT("duration")).ToSharedRef();
	return DurationJsonField->TryGetNumber(OutDuration);
}

bool UKinetixDataBlueprintFunctionLibrary::GetAnimationMetadataFromFile(UObject* WorldContextObject, FString File,
                                                                        FAnimationMetadata& AnimationMetadata)
{
	// Retrieve the Json object context
	FJsonObjectWrapper JsonObjectRef;

	if (!GetJsonObjectFromFile(WorldContextObject, File, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("File doesn't exist !"));
		return false;
	}

	if (!GetUUIDFromJson(AnimationMetadata.Id.UUID, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to get UUID !"));
		return false;
	}

	if (!GetNameFromJson(AnimationMetadata.Name, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to get Name !"));
		return false;
	}

	if (!GetOwnershipFromJson(AnimationMetadata.Ownership, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to get Ownership !"));
		return false;
	}

	if (!GetDurationFromJson(AnimationMetadata.Duration, JsonObjectRef))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("Unable to get Duration !"));
		return false;
	}

	return true;
}
