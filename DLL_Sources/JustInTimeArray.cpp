// JustInTimeArray.cpp

// usage guide is written in JustInTimeArray.h
// file written by Nightinggale

#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"

template<class T>
JustInTimeArray<T>::JustInTimeArray(JIT_ARRAY_TYPES eType, T eDefault = 0)
: tArray(NULL)
, m_iType(eType)
, m_iLength(GC.getArrayLength(eType))
, m_eDefault(eDefault)
{}

template<class T>
JustInTimeArray<T>::~JustInTimeArray()
{
	SAFE_DELETE_ARRAY(tArray);
}

template<class T>
void JustInTimeArray<T>::resetContent()
{
	if (isAllocated())
	{
		for (int iIterator = 0; iIterator < m_iLength; ++iIterator)
		{
			tArray[iIterator] = m_eDefault;
		}
	}
}

template<class T>
void JustInTimeArray<T>::releaseMemory()
{
	// TODO find best memory management
	// releasing memory here will make the game use less memory
	// however it also increases the risk of memory fragmentation
	// we will need to figure out if agressive reclaiming of memory is a good idea
#if 1
	SAFE_DELETE_ARRAY(tArray);
#else
	resetContent();
#endif
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
			// no need to allocate memory to assign a default (false) value
			return;
		}
		tArray = new T[m_iLength];
		resetContent();
	}
	tArray[iIndex] = value;
}

template<class T>
void JustInTimeArray<T>::add(T value, int iIndex)
{
	this->set(value + this->get(iIndex), iIndex);
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
bool JustInTimeArray<T>::hasContent(bool bRelease)
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

	if (bRelease)
	{
		// array is allocated but has no content
		releaseMemory();
	}
	return false;
};

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
		int iNumElements = 0;
		pStream->Read(&iNumElements);

		resetContent();

		for (int iIndex = 0; iIndex < iNumElements; iIndex++)
		{
			int iBuffer = 0;
			pStream->Read(&iBuffer);
			set(iBuffer, iIndex);
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
		releaseMemory();
	}
	else
	{
		for (int iIndex = 0; iIndex < iNumElements; iIndex++)
		{
			pStream->Write((int)get(iIndex));
		}
	}
}

template<class T>
void JustInTimeArray<T>::read(CvXMLLoadUtility* pXML, const char* sTag)
{
	// read the data into a temp int array and then set the permanent array with those values.
	// this is a workaround for template issues
	FAssert(this->m_iLength > 0);
	int *iArray = new int[this->m_iLength];
	pXML->SetVariableListTagPair(&iArray, sTag, this->m_iLength, 0);
	for (int i = 0; i < this->m_iLength; i++)
	{
		this->set(iArray[i], i);
	}
	SAFE_DELETE_ARRAY(iArray);
	this->hasContent(); // release array if possible
}


// tell the compile which template types to compile for
// has to be after all template functions (read: last in file)
template class JustInTimeArray <int>;
template class JustInTimeArray <bool>;
template class JustInTimeArray <unsigned char>;
template class JustInTimeArray <short>;
