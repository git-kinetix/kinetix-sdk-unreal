// Copyright Kinetix. All Rights Reserved.


#include "HammingWeight.h"

//------------------------------//
//             BYTE             //
//------------------------------//

const uint8 byte_m1 = 0b01010101;
const uint8 byte_m2 = 0b00110011;
const uint8 byte_m3 = 0b00001111;

uint8 HammingWeight::GetHammingWeightC(int8 a)
{
	return static_cast<uint8>(a);
}

uint8 HammingWeight::GetHammingWeightUC(uint8 a)
{
	uint8 b0 = a & byte_m1;
	uint8 b1 = (a >> 1) & byte_m1;

	uint8 b = b0 + b1;

	uint8 c0 = b & byte_m2;
	uint8 c1 = (b >> 2) & byte_m2;

	uint8 c = c0 + c1;

	uint8 d0 = c & byte_m3;
	uint8 d1 = (c >> 4) & byte_m3;

	return d0 + d1;
}

//----------------------------------------//
//             Int16 / UInt16             //
//----------------------------------------//

const uint16 short_m1 = 0b0101010101010101;
const uint16 short_m2 = 0b0011001100110011;
const uint16 short_m3 = 0b0000111100001111;
const uint16 short_m4 = 0b0000000011111111;

uint16 HammingWeight::GetHammingWeightS(int16 a)
{
	return static_cast<uint16>(a);
}

uint16 HammingWeight::GetHammingWeightUS(uint16 a)
{
	uint16 b0 = a & short_m1;
	uint16 b1 = (a >> 1) & short_m1;

	uint16 b  = b0 + b1;

	uint16 c0 = b & short_m2;
	uint16 c1 = (b >> 2) & short_m2;

	uint16 c  = c0 + c1;

	uint16 d0 = c & short_m3;
	uint16 d1 = (c >> 4) & short_m3;

	uint16 d  = d0 + d1;

	uint16 e0 = d & short_m4;
	uint16 e1 = (d >> 8) & short_m4;

	return e0 + e1;
}

//----------------------------------------//
//             Int32 / UInt32             //
//----------------------------------------//

const uint32 int_m1 = 0b01010101010101010101010101010101;
const uint32 int_m2 = 0b00110011001100110011001100110011;
const uint32 int_m3 = 0b00001111000011110000111100001111;
const uint32 int_m4 = 0b00000000111111110000000011111111;
const uint32 int_m5 = 0b00000000000000001111111111111111;

uint32 HammingWeight::GetHammingWeightL(int32 a)
{
	return static_cast<uint32>(a);
}

uint32 HammingWeight::GetHammingWeightUL(uint32 a)
{
	uint32 b0 = a & int_m1;
	uint32 b1 = (a >> 1) & int_m1;

	uint32 b = b0 + b1;

	uint32 c0 = b & int_m2;
	uint32 c1 = (b >> 2) & int_m2;

	uint32 c = c0 + c1;

	uint32 d0 = c & int_m3;
	uint32 d1 = (c >> 4) & int_m3;

	uint32 d = d0 + d1;

	uint32 e0 = d & int_m4;
	uint32 e1 = (d >> 8) & int_m4;

	uint32 e = e0 + e1;

	uint32 f0 = e & int_m5;
	uint32 f1 = (e >> 16) & int_m5;

	return f0 + f1;
}

//----------------------------------------//
//             Int64 / UInt64             //
//----------------------------------------//

const uint64 long_m1 = 0b0101010101010101010101010101010101010101010101010101010101010101;
const uint64 long_m2 = 0b0011001100110011001100110011001100110011001100110011001100110011;
const uint64 long_m3 = 0b0000111100001111000011110000111100001111000011110000111100001111;
const uint64 long_m4 = 0b0000000011111111000000001111111100000000111111110000000011111111;
const uint64 long_m5 = 0b0000000000000000111111111111111100000000000000001111111111111111;
const uint64 long_m6 = 0b0000000000000000000000000000000011111111111111111111111111111111;

uint64 HammingWeight::GetHammingWeightLL(int64 a)
{
	return static_cast<uint64>(a);
}

uint64 HammingWeight::GetHammingWeightULL(uint64 a)
{
	uint64 b0 = a & long_m1;
	uint64 b1 = (a >> 1) & long_m1;

	uint64 b = b0 + b1;

	uint64 c0 = b & long_m2;
	uint64 c1 = (b >> 2) & long_m2;

	uint64 c = c0 + c1;

	uint64 d0 = c & long_m3;
	uint64 d1 = (c >> 4) & long_m3;

	uint64 d = d0 + d1;

	uint64 e0 = d & long_m4;
	uint64 e1 = (d >> 8) & long_m4;

	uint64 e = e0 + e1;

	uint64 f0 = e & long_m5;
	uint64 f1 = (e >> 16) & long_m5;

	uint64 f = f0 + f1;

	uint64 g0 = f & long_m6;
	uint64 g1 = (f >> 32) & long_m6;

	return g0 + g1;
}
