#ifndef JUST_IN_TIME_ARRAY_H
#define JUST_IN_TIME_ARRAY_H
#pragma once
// JustInTimeArray.h

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
 */

class CvXMLLoadUtility;

template<class T> class JustInTimeArray
{
private:
	T* tArray;
	const unsigned short m_iLength;
	const unsigned short m_iType;
	const T m_eDefault;

public:
	JustInTimeArray(JIT_ARRAY_TYPES eType, T eDefault = 0);

	~JustInTimeArray();

	// reset content of an array if it is allocated
	void resetContent();

	void releaseMemory();

	// non-allocated arrays contains only default values
	// this is a really fast content check without even looking at array content
	// note that it is possible to have allocated arrays with only default content
	inline bool isAllocated() const
	{
		return tArray != NULL;
	}
	
	inline int length() const
	{
		return m_iLength;
	}

	// get stored value
	inline T get(int iIndex) const
	{
		FAssert(iIndex >= 0);
		FAssert(iIndex < m_iLength);
		return tArray ? tArray[iIndex] : m_eDefault;
	}

	// assign argument to storage
	void set(T value, int iIndex);

	// add argument to stored value
	void add(T value, int iIndex);

	// replace value with argument if argument is higher than the already stored value
	void keepMax(T value, int iIndex);

	JIT_ARRAY_TYPES getType() const;

	bool hasContent(bool bRelease = true);
	inline bool isEmpty(bool bRelease = true)
	{
		return !hasContent(bRelease);
	}

	int getNumUsedElements() const;

	// bEnable can be used like "uiFlag > x" to make oneline conditional loads
	void read(FDataStreamBase* pStream, bool bEnable = true);
	void write(FDataStreamBase* pStream);
	void read(CvXMLLoadUtility* pXML, const char* sTag);
};

template<class T>
class BonusArray: public JustInTimeArray<T>
{
public:
	BonusArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_BONUS, eDefault){};
};

template<class T>
class BuildingArray: public JustInTimeArray<T>
{
public:
	BuildingArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_BUILDING, eDefault){};
};

template<class T>
class BuildingClassArray: public JustInTimeArray<T>
{
public:
	BuildingClassArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_BUILDING_CLASS, eDefault){};
};

template<class T>
class BuildingSpecialArray: public JustInTimeArray<T>
{
public:
	BuildingSpecialArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_BUILDING_SPECIAL, eDefault){};
};

template<class T>
class CivicArray: public JustInTimeArray<T>
{
public:
	CivicArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_CIVIC, eDefault){};
};

template<class T>
class CivicOptionArray: public JustInTimeArray<T>
{
public:
	CivicOptionArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_CIVIC_OPTION, eDefault){};
};

template<class T>
class EuropeArray: public JustInTimeArray<T>
{
public:
	EuropeArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_EUROPE, eDefault){};
};

template<class T>
class FatherArray: public JustInTimeArray<T>
{
public:
	FatherArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_FATHER, eDefault){};
};

template<class T>
class HurryArray: public JustInTimeArray<T>
{
public:
	HurryArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_HURRY, eDefault){};
};

template<class T>
class ImprovementArray: public JustInTimeArray<T>
{
public:
	ImprovementArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_IMPROVEMENT, eDefault){};
};

template<class T>
class PlayerArray: public JustInTimeArray<T>
{
public:
	PlayerArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_PLAYER, eDefault){};
};

template<class T>
class ProfessionArray: public JustInTimeArray<T>
{
public:
	ProfessionArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_PROFESSION, eDefault){};
};

template<class T>
class PromotionArray: public JustInTimeArray<T>
{
public:
	PromotionArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_PROMOTION, eDefault){};
};

template<class T>
class TraitArray: public JustInTimeArray<T>
{
public:
	TraitArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_TRAIT, eDefault){};
};

template<class T>
class UnitArray: public JustInTimeArray<T>
{
public:
	UnitArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_UNIT, eDefault){};
};

template<class T>
class UnitClassArray: public JustInTimeArray<T>
{
public:
	UnitClassArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_UNIT_CLASS, eDefault){};
};

template<class T>
class UnitCombatArray: public JustInTimeArray<T>
{
public:
	UnitCombatArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_UNIT_COMBAT, eDefault){};
};

template<class T>
class YieldArray: public JustInTimeArray<T>
{
public:
	YieldArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_YIELD, eDefault){};
};

template<class T>
class YieldCargoArray: public JustInTimeArray<T>
{
public:
	YieldCargoArray(T eDefault = 0) : JustInTimeArray<T>(JIT_ARRAY_CARGO_YIELD, eDefault){};
};
#endif
