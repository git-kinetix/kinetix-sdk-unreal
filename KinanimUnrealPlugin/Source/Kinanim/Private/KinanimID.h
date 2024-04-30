// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace KinanimID
{
	const short FIELD_ID_VERSION = 0;
	const short FIELD_ID_FRAMERATE = 1;
	const short FIELD_ID_FRAMECOUNT = 2;
	const short FIELD_ID_KEYTYPES = 3;
	const short FIELD_ID_HASBLENDSHAPES = 4;
	const short FIELD_ID_ENDHEADER = static_cast<short>(static_cast<uint32>(0b1000000000000000));
	//Min short value.
}