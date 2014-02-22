// FDataStreamBase.cpp
//
// Firaxis is hiding the cpp file for FDataStreamBase inside the exe
// This file only contain the addon functions added in the mod
// Nightinggale

#include "CvGameCoreDLL.h"

// read/write 64 bit int
// this is done by spliting the 64 bit variable into 2 unsigned ints (2*32)
// using unsigned ints allows the functions to use vanilla functions.
// in other words this is just wrapper functions for vanilla functions.

void FDataStreamBase::Read(__int64 *var)
{
	unsigned int iBuffer;
	this->Read(&iBuffer);
	*var = (__int64)iBuffer << 32;
	this->Read(&iBuffer);
	*var |= iBuffer;
}

void FDataStreamBase::Read(unsigned __int64* var)
{
	unsigned int iBuffer;
	this->Read(&iBuffer);
	*var = (__int64)iBuffer << 32;
	this->Read(&iBuffer);
	*var |= iBuffer;
}

void FDataStreamBase::Write(__int64 value)
{
	unsigned int iBuffer;
	iBuffer = (unsigned int)((value >> 32) & MAX_UNSIGNED_INT);
	this->Write(iBuffer);
	iBuffer = (unsigned int)(value & MAX_UNSIGNED_INT);
	this->Write(iBuffer);
}
void FDataStreamBase::Write(unsigned __int64 value)
{
	unsigned int iBuffer;
	iBuffer = (unsigned int)((value >> 32) & MAX_UNSIGNED_INT);
	this->Write(iBuffer);
	iBuffer = (unsigned int)(value & MAX_UNSIGNED_INT);
	this->Write(iBuffer);
}