// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/Task.h"

class FKinetixEmote;
/**
 * 
 */
class FAccountPoller
{
public:
	FAccountPoller();
	~FAccountPoller();

	bool GetPollingState() const;

	void StartPolling();
	void StopPolling();

private:
	bool bPollingEnabled;
	
	UE::Tasks::TTask<void> PollingTask;
};
