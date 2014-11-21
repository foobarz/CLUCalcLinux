/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLUDraw Library.
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


// Class written by Vladimir Banarer, 3.12.2001.


// OGLBaseObject.h: Schnittstelle f�r die Klasse COGLBaseObject.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBASEOBJECT_H__2F004407_3DED_4E2C_8EBF_A36A95F26EE4__INCLUDED_)
#define AFX_OGLBASEOBJECT_H__2F004407_3DED_4E2C_8EBF_A36A95F26EE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OGLVertexList.h"

class COGLBaseObject : public COGLVertexList   
{
public:
	COGLBaseObject();
	virtual ~COGLBaseObject();

	bool Apply(int iMode = 0, void *pvData = 0);

	bool AddIdxList(int iNo, int *pIdx, int iMode);
	bool AddIdxList(Mem<int>& mIdx, int iMode);

	void Reset();

protected:

	MemObj<Mem<int> > m_moIdxListList;
	Mem<int> m_mModeList;


};

#endif // !defined(AFX_OGLBASEOBJECT_H__2F004407_3DED_4E2C_8EBF_A36A95F26EE4__INCLUDED_)
