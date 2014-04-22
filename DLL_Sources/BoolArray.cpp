// BoolArray.cpp

// usage guide is written in BoolArray.h
// file written by Nightinggale

#include "CvGameCoreDLL.h"
#include "BoolArray.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvGameAI.h"

#define BOOL_BLOCK ( iIndex >> 5 )
#define BOOL_INDEX ( iIndex & 0x1F )

BoolArray::BoolArray(JIT_ARRAY_TYPES eType, bool bDefault)
: m_iArray(NULL)
, m_iType(eType)
, m_iLength(GC.getArrayLength(eType))
, m_bDefault(bDefault)
{}

BoolArray::~BoolArray()
{
	SAFE_DELETE_ARRAY(m_iArray);
}

void BoolArray::resetContent()
{
	if (isAllocated())
	{
		int iValue = m_bDefault ? MAX_UNSIGNED_INT : 0;
		for (int iIndex = 0; iIndex < m_iLength; iIndex += 32)
		{
			m_iArray[BOOL_BLOCK] = iValue;
		}
	}
}

void BoolArray::releaseMemory()
{
	// TODO find best memory management
	// releasing memory here will make the game use less memory
	// however it also increases the risk of memory fragmentation
	// we will need to figure out if aggressive reclaiming of memory is a good idea
#if 1
	SAFE_DELETE_ARRAY(m_iArray);
#else
	resetContent();
#endif
}

bool BoolArray::get(int iIndex) const
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < m_iLength);
	return m_iArray ? HasBit(m_iArray[BOOL_BLOCK], BOOL_INDEX) : m_bDefault;
}

void BoolArray::set(bool bValue, int iIndex)
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < m_iLength);

	if (m_iArray == NULL)
	{
		if (bValue == m_bDefault)
		{
			// no need to allocate memory to assign a default (false) value
			return;
		}
		int iLength = m_iLength / 32;
		if (m_iLength % 32)
		{
			iLength++;
		}
		m_iArray = new unsigned int[iLength];
		resetContent();
	}
	
	if (bValue)
	{
		m_iArray[BOOL_BLOCK] |= SETBIT(BOOL_INDEX);
	}
	else
	{
		m_iArray[BOOL_BLOCK] &= ~SETBIT(BOOL_INDEX);
	}
	
}

JIT_ARRAY_TYPES BoolArray::getType() const
{
	return (JIT_ARRAY_TYPES)m_iType;
}

bool BoolArray::hasContent(bool bRelease)
{
	if (m_iArray == NULL)
	{
		return false;
	}
	for (int iIterator = 0; iIterator < m_iLength; ++iIterator)
	{
		if (get(iIterator) == m_bDefault)
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

int BoolArray::getNumUsedElements() const
{
	int iMax = 0;
	if (isAllocated())
	{
		for (unsigned int iIndex = 0; iIndex < m_iLength; iIndex++)
		{
			if (get(iIndex) != m_bDefault)
			{
				iMax = 1 + iIndex;
			}
		}
	}
	return iMax;
}

void BoolArray::read(FDataStreamBase* pStream, bool bEnable)
{
	if (bEnable)
	{
		CvGameAI& eGame = GC.getGameINLINE();

		int iNumElements = 0;
		pStream->Read(&iNumElements);

		resetContent();

		unsigned int iBuffer;

		for (int iIndex = 0; iIndex < iNumElements; iIndex++)
		{
			if (BOOL_INDEX == 0)
			{
				pStream->Read(&iBuffer);
			}
			int iNewIndex = eGame.convertArrayInfo(getType(), iIndex);
			if (iNewIndex >= 0)
			{
				set(HasBit(iBuffer, BOOL_INDEX), iNewIndex);
			}
		}
	}
}

void BoolArray::write(FDataStreamBase* pStream)
{
	int iNumElements = getNumUsedElements();

	pStream->Write(iNumElements);

	if (iNumElements == 0)
	{
		releaseMemory();
	}
	else
	{
		for (int iIndex = 0; iIndex < iNumElements; iIndex += 32)
		{
			pStream->Write(m_iArray[BOOL_BLOCK]);
		}
	}
}

void BoolArray::read(CvXMLLoadUtility* pXML, const char* sTag)
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

