#pragma once

//	$Revision: #4 $		$Author: mbreitkreutz $ 	$DateTime: 2005/06/13 13:35:55 $
//---------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

/*
*
* FILE:    FDataStreamBase.h
* DATE:    7/15/2004
* AUTHOR: Mustafa Thamer
* PURPOSE: Base Classe of Stream classes for file, null and mem streams.
*
*		Easily read and write data to those kinds of streams using the same baseclass interface.
*		FStrings not used since this is a public dll header.
*
*/

#ifndef		FDATASTREAMBASE_H
#define		FDATASTREAMBASE_H
#pragma		once

//
// Stream abstract base class
//
class FDataStreamBase
{
public:
	virtual void	Rewind() = 0;
	virtual bool	AtEnd() = 0;
	virtual void	FastFwd() = 0;
	virtual unsigned int  GetPosition() const =  0;
	virtual void    SetPosition(unsigned int position) = 0;
	virtual void    Truncate() = 0;
	virtual void	Flush() = 0;
	virtual unsigned int	GetEOF() const = 0;
	virtual unsigned int			GetSizeLeft() const = 0;
	virtual void	CopyToMem(void* mem) = 0;

	virtual unsigned int	WriteString(const wchar *szName) = 0;
	virtual unsigned int	WriteString(const char *szName) = 0;
	virtual unsigned int	WriteString(const std::string& szName) = 0;
	virtual unsigned int	WriteString(const std::wstring& szName) = 0;
	virtual unsigned int	WriteString(int count, std::string values[]) = 0;
	virtual unsigned int	WriteString(int count, std::wstring values[]) = 0;

	virtual unsigned int	ReadString(char *szName) = 0;
	virtual unsigned int	ReadString(wchar *szName) = 0;
	virtual unsigned int	ReadString(std::string& szName) = 0;
	virtual unsigned int	ReadString(std::wstring& szName) = 0;
	virtual unsigned int	ReadString(int count, std::string values[]) = 0;
	virtual unsigned int	ReadString(int count, std::wstring values[]) = 0;

	virtual char *			ReadString() = 0;		// allocates memory
	virtual wchar *		ReadWideString() = 0;	// allocates memory

	virtual void		Read(char *) = 0;
	virtual void		Read(byte *) = 0;
	virtual void		Read(int count, char values[]) = 0;
	virtual void		Read(int count, byte values[]) = 0;
	virtual void		Read(bool *) = 0;
	virtual void		Read(int count, bool values[]) = 0;
	virtual void		Read(short	*s) = 0;
	virtual void		Read(unsigned short	*s)  = 0;
	virtual void		Read(int count, short values[]) = 0;
	virtual void		Read(int count, unsigned short values[]) = 0;
	virtual void		Read(int* i) = 0;
	virtual void		Read(unsigned int* i) = 0;
	virtual void 		Read(int count, int values[]) = 0;
	virtual void 		Read(int count, unsigned int values[]) = 0;

	virtual void		Read(long* l) = 0;
	virtual void		Read(unsigned long* l)  = 0;
	virtual void 		Read(int count, long values[]) = 0;
	virtual void 		Read(int count, unsigned long values[])  = 0;

	virtual void		Read(float* value) = 0;
	virtual void		Read(int count, float values[]) = 0;

	virtual void		Read(double* value) = 0;
	virtual void		Read(int count, double values[]) = 0;

	virtual void		Write( char value) = 0;
	virtual void		Write(byte value) = 0;
	virtual void		Write(int count, const  char values[]) = 0;
	virtual void		Write(int count, const  byte values[]) = 0;

	virtual void		Write(bool value) = 0;
	virtual void		Write(int count, const bool values[]) = 0;

	virtual void		Write(short value) = 0;
	virtual void		Write(unsigned short value) = 0;
	virtual void		Write(int count, const short values[]) = 0;
	virtual void		Write(int count, const unsigned short values[])  = 0;

	virtual void		Write(int value) = 0;
	virtual void		Write(unsigned int value)  = 0;
	virtual void 		Write(int count, const int values[]) = 0;
	virtual void		Write(int count, const unsigned int values[])  = 0;

	virtual void		Write(long value) = 0;
	virtual void		Write(unsigned long  value)  = 0;
	virtual void 		Write(int count, const long values[]) = 0;
	virtual void		Write(int count, const unsigned long values[])  = 0;

	virtual void		Write(float value) = 0;
	virtual void		Write(int count, const float values[]) = 0;

	virtual void		Write(double value) = 0;
	virtual void		Write(int count, const double values[]) = 0;

	/// 64 bit save - start - Nightinggale
	void                Read(__int64* variable);
	void                Read(unsigned __int64* variable);
	void		        Write(__int64 value);
	void		        Write(unsigned __int64 value);
	/// 64 bit save - end - Nightinggale

	/// JIT array save - start - Nightinggale
	void                Read(JIT_ARRAY_TYPES eType, int* i);
	
	void Read(BonusTypes*           eVar) {Read(JIT_ARRAY_BONUS            , (int*) eVar);}
	void Read(BuildTypes*           eVar) {Read(JIT_ARRAY_BUILD            , (int*) eVar);}
	void Read(BuildingTypes*        eVar) {Read(JIT_ARRAY_BUILDING         , (int*) eVar);}
	void Read(BuildingClassTypes*   eVar) {Read(JIT_ARRAY_BUILDING_CLASS   , (int*) eVar);}
	void Read(SpecialBuildingTypes* eVar) {Read(JIT_ARRAY_BUILDING_SPECIAL , (int*) eVar);}
	void Read(CivicTypes*           eVar) {Read(JIT_ARRAY_CIVIC            , (int*) eVar);}
	void Read(EraTypes*             eVar) {Read(JIT_ARRAY_ERA              , (int*) eVar);}
	void Read(EmphasizeTypes*       eVar) {Read(JIT_ARRAY_EMPHASIZE        , (int*) eVar);}
	void Read(EuropeTypes*          eVar) {Read(JIT_ARRAY_EUROPE           , (int*) eVar);}
	void Read(EventTriggerTypes*    eVar) {Read(JIT_ARRAY_EVENT_TRIGGER    , (int*) eVar);}
	void Read(FatherTypes*          eVar) {Read(JIT_ARRAY_FATHER           , (int*) eVar);}
	void Read(FeatureTypes*         eVar) {Read(JIT_ARRAY_FEATURE          , (int*) eVar);}
	void Read(HandicapTypes*        eVar) {Read(JIT_ARRAY_HANDICAP         , (int*) eVar);}
	void Read(ImprovementTypes*     eVar) {Read(JIT_ARRAY_IMPROVEMENT      , (int*) eVar);}
	void Read(LeaderHeadTypes*      eVar) {Read(JIT_ARRAY_LEADER_HEAD      , (int*) eVar);}
	void Read(ProfessionTypes*      eVar) {Read(JIT_ARRAY_PROFESSION       , (int*) eVar);}
	void Read(PromotionTypes*       eVar) {Read(JIT_ARRAY_PROMOTION        , (int*) eVar);}
	void Read(RouteTypes*           eVar) {Read(JIT_ARRAY_ROUTE            , (int*) eVar);}
	void Read(TerrainTypes*         eVar) {Read(JIT_ARRAY_TERRAIN          , (int*) eVar);}
	void Read(UnitTypes*            eVar) {Read(JIT_ARRAY_UNIT             , (int*) eVar);}
	void Read(UnitClassTypes*       eVar) {Read(JIT_ARRAY_UNIT_CLASS       , (int*) eVar);}
	void Read(UnitCombatTypes*      eVar) {Read(JIT_ARRAY_UNIT_COMBAT      , (int*) eVar);}
	void Read(SpecialUnitTypes*     eVar) {Read(JIT_ARRAY_UNIT_SPECIAL     , (int*) eVar);}
	void Read(YieldTypes*           eVar) {Read(JIT_ARRAY_YIELD            , (int*) eVar);}
	/// JIT array save - end - Nightinggale
};

#endif	//FDATASTREAMBASE_H
