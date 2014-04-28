// BoolArray.cpp

// usage guide is written in BoolArray.h
// file written by Nightinggale

#include "CvGameCoreDLL.h"
#include "BoolArray.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvGameAI.h"
#include "MemoryManager.h"

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

void BoolArray::reset()
{
	int iLength = (m_iLength + 31) / 32;

	MMreleaseMemory(&m_iArray, iLength);

	return;

	if (isAllocated())
	{
		int iValue = m_bDefault ? MAX_UNSIGNED_INT : 0;
		for (int iIndex = 0; iIndex < m_iLength; iIndex += 32)
		{
			m_iArray[BOOL_BLOCK] = iValue;
		}
	}
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
		if (!bValue == !m_bDefault)
		{
			// no need to allocate memory to assign a default value
			return;
		}
		int iLength = (m_iLength + 31) / 32;
		MMallocateMemory(&m_iArray, iLength);
		for (int i = 0; i < iLength; i++)
		{
			m_iArray[i] = m_bDefault ? MAX_UNSIGNED_INT : 0;
		}
	}
	
	if (bValue)
	{
		m_iArray[BOOL_BLOCK] |= SETBIT(BOOL_INDEX);
	}
	else
	{
		m_iArray[BOOL_BLOCK] &= ~SETBIT(BOOL_INDEX);
	}
	
	if (!bValue != !m_bDefault)
	{
		hasContent();
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

	int iLength = (m_iLength + 31) / 32;
	for (int i = 0; i < iLength; i++)
	{
		if (m_bDefault)
		{
			if (~m_iArray[i])
			{
				return true;
			}
		}
		else
		{
			if (m_iArray[i])
			{
				return true;
			}
		}
	}

	reset();
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

		reset();

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
		reset();
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
	int *iArray;
	MMallocateMemory(&iArray, m_iLength);
	pXML->SetVariableListTagPair(&iArray, sTag, this->m_iLength, 0);
	for (int i = 0; i < this->m_iLength; i++)
	{
		this->set(iArray[i], i);
	}
	MMreleaseMemory(&iArray, m_iLength);
	this->hasContent(); // release array if possible
}

