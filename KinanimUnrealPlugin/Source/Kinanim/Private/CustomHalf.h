// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
namespace CustomHalf
{
	const int SIZEOF = sizeof(int16);

	float From(int16 S);
	int16 To(float F);
	
}