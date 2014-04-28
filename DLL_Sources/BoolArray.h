#ifndef BOOL_ARRAY_H
#define BOOL_ARRAY_H
#pragma once
// BoolArray.h

/*
 * BoolArray is used more or less like a just-in-time array.
 * Functions are named the same and to the outside world they behave the same. 
 */

class CvXMLLoadUtility;

class BoolArray
{
private:
	unsigned int* m_iArray;
	const unsigned short m_iLength;
	const unsigned char m_iType;
	const bool m_bDefault : 1;

public:
	BoolArray(JIT_ARRAY_TYPES eType, bool bDefault = false);

	~BoolArray();

	// reset content of an array if it is allocated
	void reset();

	// non-allocated arrays contains only default values
	// this is a really fast content check without even looking at array content
	// note that it is possible to have allocated arrays with only default content
	inline bool isAllocated() const
	{
		return m_iArray != NULL;
	}
	
	inline int length() const
	{
		return m_iLength;
	}

	// get stored value
	bool get(int iIndex) const;

	// assign argument to storage
	void set(bool bValue, int iIndex);

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
#endif
