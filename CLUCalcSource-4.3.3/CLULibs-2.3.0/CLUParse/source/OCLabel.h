/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUParse Library.
//
// 
// Copyright (C) 2001-2004  Christian B.U. Perwass
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The GNU Library General Public License can be found in the file
// license.txt distributed with this library.
//
// Please send all queries and comments to
//
// email: help@clucalc.info
// mail: Institut fuer Informatik, Olshausenstr. 40, 24098 Kiel, Germany.
//
////////////////////////////////////////////////////////////////////////////////////
/// LICENSE END

// OCLabel.h: Schnittstelle f�r die Klasse COCLabel.
//
//	This class defines the parse behaviour of the object class of labels.
//	A label is a continuous string of symbols within a certain
//	subset of symbols. The subset of symbols used is given in
//	m_csSymbolSpace. 
//
//	This class creates an instance of CCodeLabel which implements
//	the functionality of lables in general. 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCLABEL_H__A469F097_64ED_45D7_BE36_6FEBAE298405__INCLUDED_)
#define AFX_OCLABEL_H__A469F097_64ED_45D7_BE36_6FEBAE298405__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectClass.h"

class COCLabel : public CObjectClass  
{
public:
	COCLabel();
	virtual ~COCLabel();

	void SetSymbolSpace(const char *pcSS) { if (pcSS) m_csSymbolSpace = pcSS; }

	virtual bool Parse(CParseBase& rParse, CCodeElementList& rCode);

	// If Label refers to a function then this function is called
	// to parse the rest.
	bool ParseFunc(CCodeElement *pFunc, CParseBase& rParse, CCodeElementList& rCode);

protected:
	// Symbols that follow the first ID symbol do not need to be unique to this class.
	cStr m_csSymbolSpace;

};

#endif // !defined(AFX_OCLABEL_H__A469F097_64ED_45D7_BE36_6FEBAE298405__INCLUDED_)
