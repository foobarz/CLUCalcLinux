/// LICENSE START
////////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the CLU Library.
//
// 
// Copyright (C) 1997-2004  Christian B.U. Perwass
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

#include "TensorContractLoop.h"

////////////////////////////////////////////////////////////////////////////////////
/// Single Loop Class Member Functions
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
/// Constructor

template <class CType>
CTensorContractLoop<CType>::CTensorContractLoop() : CTensorDoubleLoop<CType>()
{
}


////////////////////////////////////////////////////////////////////////////////////
/// Set Tensor Idx Instance

template <class CType>
void CTensorContractLoop<CType>::Set(CTensorData<CType>& rTResult, CTensorIdx<CType>& rTLeft, CTensorIdx<CType>& rTRight) 
	throw (CCLUException)
{
	Mem<int> mDim, mIdx;
	int iLoopCount, iLoop;

	CTensorDoubleLoop<CType>::Set(rTLeft, rTRight);
	iLoopCount = CTensorDoubleLoop<CType>::Init();

	for(iLoop = iLoopCount-1; iLoop >= 0; iLoop--)
	{
		// Since we contract, double loops don't count.
		// However, if the corresponding free index is smaller than MIN_CONTRACT_IDX,
		// this is not a contraction loop but a point loop. Hence, count it.
		if (!IsDoubleLoop(iLoop) || this->FreeIdx(iLoop) < MIN_CONTRACT_IDX)
		{
			mDim.Add(1);
			mDim.Last() = StepCount(iLoop);

			mIdx.Add(1);
			mIdx.Last() = this->FreeIdx(iLoop);
		}
	}

	if (mDim.Count() == 0)
	{
		// Full contraction to single value
		mDim.Set(1);
		mDim[0] = 1;

		mIdx.Set(1);
		mIdx[0] = 0;
	}

	rTResult.Reset(mDim);
	m_ResultTIdx.Set(rTResult, mIdx);
}




////////////////////////////////////////////////////////////////////////////////////
// Initialize Loops. Returns number of cascaded loops.

template <class CType>
int CTensorContractLoop<CType>::Init() 
	throw (CCLUException)
{
	//m_ResultLoop.Init();
	m_ResultTIdx.InitAllLoops();

	return CTensorDoubleLoop<CType>::Init();
}



////////////////////////////////////////////////////////////////////////////////////
// Step loop. Returns loop level, in which step was made.
// Loop level starts at zero for most inner loop.
// If loop level is -1, all loops are finished.
// No exceptions generated by this function!

template <class CType>
int CTensorContractLoop<CType>::Step() 
{
	int iLevel = 0;

	if (this->m_mFreeIdx.Count() == 0)
		return -1;

	while (true)
	{
		typename CTensorContractLoop<CType>::SIdxData &rIdx = this->m_mFreeIdx[iLevel];
		if (rIdx.bBoth)
		{
			// If this is a double index, but also a point index,
			// then do not contract over this index.
			if (rIdx.iFreeIdx < MIN_CONTRACT_IDX)
				m_ResultTIdx.StepLoop(rIdx.iFreeIdx);

			rIdx.pTFirst->StepLoop(rIdx.iFreeIdx);
			if (rIdx.pTSecond->StepLoop(rIdx.iFreeIdx))
				break;

			rIdx.pTFirst->InitLoop(rIdx.iFreeIdx);
			rIdx.pTSecond->InitLoop(rIdx.iFreeIdx);

			if (rIdx.iFreeIdx < MIN_CONTRACT_IDX)
				m_ResultTIdx.InitLoop(rIdx.iFreeIdx);
		}
		else
		{
			m_ResultTIdx.StepLoop(rIdx.iFreeIdx);
			if (rIdx.pTFirst->StepLoop(rIdx.iFreeIdx))
				break;

			m_ResultTIdx.InitLoop(rIdx.iFreeIdx);
			rIdx.pTFirst->InitLoop(rIdx.iFreeIdx);
		}

		if (iLevel == this->m_mFreeIdx.Count() - 1)
			return -1;	// All loops finished

		++iLevel;
	}

	return iLevel;
}

