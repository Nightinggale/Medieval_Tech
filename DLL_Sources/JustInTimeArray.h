#pragma once
// JustInTimeArray.h

#include "CvDLLEntity.h"
#include "CvGlobals.h"

/*
 * Guide for just-in-time array usage
 *  This is classes containing a single pointer. This means from a coding point of view they can be used like an array.
 *  The main difference from normal arrays is that they will not allocate memory until a non-default value is written in it.
 *  This is useful for arrays where say only human players will use them. AI players will then not allocate the memory at all.
 *  The length of the array is hardcoded into the class, eliminating the risk of assuming wrong length.
 *
 *  There is one other major difference between arrays and JustInTimeArrays.
 *    Normal arrays have fixed sized (compiletime) when placed in classes or they are pointers to arrays,
 *     in which case they leak memory unless SAFE_DELETE_ARRAY() is used and they need to be allocated
 *    JustInTimeArrays have the good part of both. Length is set at runtime, yet they can still be added as a variable
 *      to classes, in which case allocation and freeing of memory is automatic and can't leak. 
 *
 *   IMPORTANT:
 *    An array of JustInTimeArrays fail to activate constructor/deconstructor. reset() and init() must be used here
 *     Failure to do so will leak memory and/or fail to set length.
 *
 *
 *  Usage:
 *   get() reads data from array. 0 or false is returned when read from an unallocated array.
 *   set() writes data to array. Allocates if needed.
 *   hasContent() and isEmpty() tells if the array is allocated.
 *     They also frees the array if it only contains 0/false.
 *   isAllocated() returns true if the array is allocated but doesn't check contents (faster than hasContent())
 *   read()/write() used for savegames. The bool tells if the data should be read/written.
 *      This allows one-line statements when saving, which includes the if sentence.
 *   reset() frees the array
 *   length() is the length of the array
 *   init() does the same as the constructor and should be used when the constructor isn't called, like in an array of JustInTimeArrays
 *
 *  Adding new types:
 *   Copy class YieldArray and change:
 *    2 * YieldArray (function and constructor) into a suitable name
*     2 * NUM_YIELD_TYPES into whatever length you need.
 *
 *  The reason why the class function definitions are inside the declaration is to avoid compiler errors regarding templates.
 */

template<class T> class JustInTimeArray
{
private:
	T* tArray;
	const int m_iLength;
	const T m_eDefault;

public:
	JustInTimeArray(int iLength, T eDefault = 0)
	: tArray(NULL)
	, m_iLength(iLength)
	, m_eDefault(eDefault)
	{}

	~JustInTimeArray()
	{
		SAFE_DELETE_ARRAY(tArray);
	}

	// use resetContent() if you want to write to the array
	// this way memory will not be freed and reallocated (slow process)
	inline void reset()
	{
		SAFE_DELETE_ARRAY(tArray);
	}

	// reset content of an array if it is allocated
	inline void resetContent()
	{
		if (isAllocated())
		{
			for (int iIterator = 0; iIterator < m_iLength; ++iIterator)
			{
				tArray[iIterator] = m_eDefault;
			}
		}
	}

	inline bool isAllocated() const
	{
		return tArray != NULL;
	}
	
	inline int length()
	{
		return m_iLength;
	}

	inline T get(int iIndex) const
	{
		FAssert(iIndex >= 0);
		FAssert(iIndex < m_iLength);
		return tArray ? tArray[iIndex] : m_eDefault;
	}

	inline void set(T value, int iIndex)
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

	inline void add(T value, int iIndex)
	{
		this->set(value + this->get(iIndex), iIndex);
	}

	inline void keepMax(T value, int iIndex)
	{
		if (value > get(iIndex))
		{
			set(value, iIndex);
		}
	}

	bool hasContent(bool bRelease = true)
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
	inline bool isEmpty(bool bRelease = true)
	{
		return !hasContent(bRelease);
	}

	void read(FDataStreamBase* pStream, bool bRead)
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

	void write(FDataStreamBase* pStream, bool bWrite)
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

	void read(CvXMLLoadUtility* pXML, const char* sTag)
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
};


template<class T>
class YieldArray: public JustInTimeArray<T>
{
public:
	YieldArray() : JustInTimeArray<T>(NUM_YIELD_TYPES){};
	YieldArray(T eDefault) : JustInTimeArray<T>(NUM_YIELD_TYPES, eDefault){};
};

template<class T>
class YieldCargoArray: public JustInTimeArray<T>
{
public:
	YieldCargoArray() : JustInTimeArray<T>(NUM_CARGO_YIELD_TYPES){};
	YieldCargoArray(T eDefault) : JustInTimeArray<T>(NUM_CARGO_YIELD_TYPES, eDefault){};
};

template<class T>
class UnitArray: public JustInTimeArray<T>
{
public:
    UnitArray() : JustInTimeArray<T>(GC.getNumUnitInfos()){};
	UnitArray(T eDefault) : JustInTimeArray<T>(GC.getNumUnitInfos(), eDefault){};
};

template<class T>
class ProfessionArray: public JustInTimeArray<T>
{
public:
    ProfessionArray() : JustInTimeArray<T>(GC.getNumProfessionInfos()){};
	ProfessionArray(T eDefault) : JustInTimeArray<T>(GC.getNumProfessionInfos(), eDefault){};
};

template<class T>
class PromotionArray: public JustInTimeArray<T>
{
public:
    PromotionArray() : JustInTimeArray<T>(GC.getNumPromotionInfos()){};
	PromotionArray(T eDefault) : JustInTimeArray<T>(GC.getNumPromotionInfos(), eDefault){};
};

template<class T>
class UnitCombatArray: public JustInTimeArray<T>
{
public:
    UnitCombatArray() : JustInTimeArray<T>(GC.getNumUnitCombatInfos()){};
	UnitCombatArray(T eDefault) : JustInTimeArray<T>(GC.getNumUnitCombatInfos(), eDefault){};
};

template<class T>
class BonusArray: public JustInTimeArray<T>
{
public:
    BonusArray() : JustInTimeArray<T>(GC.getNumBonusInfos()){};
	BonusArray(T eDefault) : JustInTimeArray<T>(GC.getNumBonusInfos(), eDefault){};
};

template<class T>
class PlayerArray: public JustInTimeArray<T>
{
public:
	PlayerArray() : JustInTimeArray<T>(MAX_PLAYERS){};
	PlayerArray(T eDefault) : JustInTimeArray<T>(MAX_PLAYERS, eDefault){};
};

template<class T>
class EuropeArray: public JustInTimeArray<T>
{
public:
	EuropeArray() : JustInTimeArray<T>(GC.getNumEuropeInfos()){};
	EuropeArray(T eDefault) : JustInTimeArray<T>(GC.getNumEuropeInfos(), eDefault){};
};

template<class T>
class BuildingArray: public JustInTimeArray<T>
{
public:
    BuildingArray() : JustInTimeArray<T>(GC.getNumBuildingInfos()){};
	BuildingArray(T eDefault) : JustInTimeArray<T>(GC.getNumBuildingInfos(), eDefault){};
};
