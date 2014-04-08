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
void JustInTimeArray<T>::reset()
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
		SAFE_DELETE_ARRAY(tArray);
	}
	return false;
};

template<class T>
void JustInTimeArray<T>::read(FDataStreamBase* pStream, bool bRead)
{
	if (bRead)
	{
		if (tArray == NULL)
		{
			tArray = new T[m_iLength];
		}
		pStream->Read(m_iLength, tArray);
	}
}

template<class T>
void JustInTimeArray<T>::write(FDataStreamBase* pStream, bool bWrite)
{
	if (bWrite)
	{
		if (tArray == NULL)
		{
			// requested writing an empty array.
			for (int i = 0; i < m_iLength; i++)
			{
				pStream->Write(m_eDefault);
			}
		} else {
			pStream->Write(m_iLength, tArray);
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
