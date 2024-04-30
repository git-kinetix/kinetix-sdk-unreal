// Copyright Kinetix. All Rights Reserved.


#include "CustomHalf.h"

const float TO_SHORT = 10e3f;
const float TO_FLOAT = 1 / TO_SHORT;

float CustomHalf::From(int16 S)
{
	return S * TO_FLOAT;
}

int16 CustomHalf::To(float F)
{
	return F * TO_SHORT;
}
