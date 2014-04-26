#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#pragma once
// MemoryManager.h

// use these functions to allocate and free memory for arrays.
// The memory manager will then try to reuse already allocated memory.
// This will reduce memory operations and memory fragmentation.

// don't call functions directly. Use the template function
void MMreleaseMemory(int* pPointer, unsigned int iLength, unsigned int iSize);
int* MMallocateMemory(unsigned int iLength);

template <typename T>
static inline void MMreleaseMemory(T** ppPointer, unsigned int iLength)
{
	MMreleaseMemory((int*) *ppPointer, iLength, sizeof(T));
	*ppPointer = NULL;
}

template <typename T>
static inline void MMallocateMemory(T** ppPointer, unsigned int iLength)
{
	*ppPointer = (T*)MMallocateMemory(iLength * sizeof(T));
}

#endif
