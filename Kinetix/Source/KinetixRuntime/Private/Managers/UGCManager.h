// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/UniquePtr.h"

/**
 * 
 */
class KINETIXRUNTIME_API FUGCManager
{
public:
	
	FUGCManager();
	~FUGCManager();

	static FUGCManager& Get();
	
private:

	static TUniquePtr<FUGCManager> Instance;

};
