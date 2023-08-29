// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/KinetixDataLibrary.h"
#include "Tasks/Task.h"
#include "Templates/UniquePtr.h"

class UTexture2DDynamic;

/**
 * 
 */
class KINETIXRUNTIME_API FIconManager
{
public:
	FIconManager();
	~FIconManager();

	static FIconManager& Get();

	UTexture2DDynamic* GetIcon(const FAnimationID& ID, const FOnIconAvailable& OnIconAvailableDelegate);
	void AddIcon(const FAnimationID& InID, UTexture2DDynamic* Texture);
	
private:

	static TUniquePtr<FIconManager> Instance;
	
	UE::Tasks::TTask<bool> SampleTask;
	
	TMap<FAnimationID, UTexture2DDynamic*> IconTexturesByID;
};
