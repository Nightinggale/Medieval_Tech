// MemoryManager.cpp

// this file keeps track of memory allocations and will attempt to reuse it

#include "CvGameCoreDLL.h"
#include "MemoryManager.h"


std::vector< std::list<int*> > m_vector;

void MMreleaseMemory(int* pPointer, unsigned int iLength, unsigned int iSize)
{
	if (pPointer == NULL)
	{
		// NULL pointers should be ignored as the array isn't even allocated in the first place
		return;
	}

	iLength *= iSize;
	iLength += 3;
	iLength /= 4;

	while (m_vector.size() <= iLength)
	{
		std::list<int*> list;
		m_vector.push_back(list);
	}

	m_vector[iLength].push_front(pPointer);
}

int* MMallocateMemory(unsigned int iLength)
{
	iLength += 3;
	iLength /= 4;

	if (m_vector.size() > iLength && !m_vector[iLength].empty())
	{
		int* pPointer = *m_vector[iLength].begin();
		m_vector[iLength].pop_front();
		return pPointer;
	}

	return new int[iLength];
}
