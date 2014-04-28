// JustInTimeArray.cpp

// usage guide is written in JustInTimeArray.h
// file written by Nightinggale

#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvGameAI.h"
#include "MemoryManager.h"

template<class T>
JustInTimeArray<T>::JustInTimeArray(JIT_ARRAY_TYPES eType, T eDefault)
: tArray(NULL)
, m_iType(eType)
, m_iLength(GC.getArrayLength(eType))
, m_eDefault(eDefault)
{
	FAssertMsg(m_iLength != 0, "arrays can't have 0 length");
}

template<class T>
JustInTimeArray<T>::~JustInTimeArray()
{
	reset();
}

template<class T>
void JustInTimeArray<T>::reset()
{
	MMreleaseMemory(&tArray, m_iLength);
}

template<class T>
void JustInTimeArray<T>::set(T value, int iIndex)
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < m_iLength);

	if (tArray == NULL)
	{
		if (value == m_eDefault)
		{
			// no need to allocate memory to assign a default value
			return;
		}
		MMallocateMemory(&tArray, m_iLength);
		for (int iIterator = 0; iIterator < m_iLength; ++iIterator)
		{
			tArray[iIterator] = m_eDefault;
		}
	}
	tArray[iIndex] = value;

	if (value == m_eDefault)
	{
		// release memory if it only has default values left
		hasContent();
	}
}

template<class T>
void JustInTimeArray<T>::add(T value, int iIndex)
{
	this->set((T)(value + get(iIndex)), iIndex);
}

template<class T>
void JustInTimeArray<T>::keepMax(T value, int iIndex)
{
	if (value > get(iIndex))
	{
		set(value, iIndex);
	}
}

template<class T>
JIT_ARRAY_TYPES JustInTimeArray<T>::getType() const
{
	return (JIT_ARRAY_TYPES)m_iType;
}

template<class T>
bool JustInTimeArray<T>::hasContent()
{
	if (tArray == NULL)
	{
		return false;
	}
	for (int iIterator = 0; iIterator < m_iLength; ++iIterator)
	{
		if (tArray[iIterator])
		{
			return true;
		}
	}

	reset();
	return false;
}

template<class T>
int JustInTimeArray<T>::getNumUsedElements() const
{
	int iMax = 0;
	if (isAllocated())
	{
		for (unsigned int iIndex = 0; iIndex < m_iLength; iIndex++)
		{
			if (tArray[iIndex] != m_eDefault)
			{
				iMax = 1 + iIndex;
			}
		}
	}
	return iMax;
}

template<class T>
void JustInTimeArray<T>::read(FDataStreamBase* pStream, bool bEnable)
{
	if (bEnable)
	{
		CvGameAI& eGame = GC.getGameINLINE();

		int iNumElements = 0;
		pStream->Read(&iNumElements);

		reset();

		for (int iIndex = 0; iIndex < iNumElements; iIndex++)
		{
			T iBuffer = (T)0;
			pStream->Read(&iBuffer);
			int iNewIndex = eGame.convertArrayInfo(getType(), iIndex);
			if (iNewIndex >= 0)
			{
				set(iBuffer, iNewIndex);
			}
		}
	}
}

template<class T>
void JustInTimeArray<T>::write(FDataStreamBase* pStream)
{
	int iNumElements = getNumUsedElements();

	pStream->Write(iNumElements);

	if (iNumElements == 0)
	{
		reset();
	}
	else
	{
		for (int iIndex = 0; iIndex < iNumElements; iIndex++)
		{
			pStream->Write(get(iIndex));
		}
	}
}

template<class T>
void JustInTimeArray<T>::read(CvXMLLoadUtility* pXML, const char* sTag)
{
	// read the data into a temp int array and then set the permanent array with those values.
	// this is a workaround for template issues
	FAssert(this->m_iLength > 0);
	int *iArray;
	MMallocateMemory(&iArray, m_iLength);
	pXML->SetVariableListTagPair(&iArray, sTag, this->m_iLength, 0);
	for (int i = 0; i < this->m_iLength; i++)
	{
		this->set((T)iArray[i], i);
	}
	MMreleaseMemory(&iArray, m_iLength);
	this->hasContent(); // release array if possible
}


// tell the compile which template types to compile for
// has to be after all template functions (read: last in file)

// IMPORTANT: do not make one with <bool>
// BoolArray should always do the job better than JIT<bool>

template class JustInTimeArray <int>;
template class JustInTimeArray <char>;
template class JustInTimeArray <unsigned char>;
template class JustInTimeArray <short>;

// array types
// keep the amount of these to a minimum
// they do not share the compiled code with int even though they do the same
// this mean dublicated code in the DLL
template class JustInTimeArray <CivicTypes>;