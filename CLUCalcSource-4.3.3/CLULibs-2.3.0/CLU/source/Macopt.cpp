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

//#include "r.h" 


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>


#include "Macopt.h"

/* 

	http://131.111.48.24/mackay/c/macopt.html       mackay@mrao.cam.ac.uk
  
	Please do not use macopt without understanding a little about how it works;
	there are some control parameters which the user MUST set!
	
	David MacKay's optimizer, based on conjugate gradient ideas, 
	but using bracketing of the zero of the inner product 
	  
	(gradient).(line_search_direction)
		
	to do the line minimization. Only derivative calculations are required.
	The length of the first step in the line search (often set to "1.0"
	in other code) is adapted here so that, if 0.00001 is a better step size, 
	it soon cottons on to that and saves ~log(10000) bracketing operations.
	The result is that (with rich set to 0) the program can use 
	as few as 2 derivatives per line search. (If rich is set to 1, it does 
	an extra derivative calculation at the beginning of each line search 
	making a minimum of 3 per line search. Set rich=0 if you think 
	that the surface is locally quite quadratic.) If the program does average 
	2 derivatives per line search then it must be superior to most cg methods 
	including use of Rbackprop (which costs 2 derivatives straight off)
		  
	A possible modification: where the function can be returned at same 
	time as the dfunction --- there is nothing clever to do with the 
	value, but it could be used as a sanity check and a convergence criterion. 
			
	See http://131.111.48.24/mackay/c/macopt.html for further discussion.
			  
	NB: The value of "tol" is totally arbitrary and must be set by 
	you to a value that works well for your problem. 
	It depends completely on the typical value of the gradient / step size. 
				
	Tol specifies a magnitude of gradient at which a halt is called. 
	or a step size.
				  
	This program MINIMIZES a function.
					
					  
	**********************************************
					
	Modified and converted to C++ class by Steve Waterhouse 8th April
	1997.
					  
	**********************************************
		
	//////////////////////////////////////////////
	
	Modified (and debugged) to work with XMU-Library by
	Christian B.U. Perwass, August 1999.

	Indices now count from 0 to n-1 instead of 1 to n !!!

	//////////////////////////////////////////////

*/
////////////////////////////////////////////////////////////////////////////////
/// Constructor

MacOpt::MacOpt()
{
	errList.AddMessage(ERR_NONE, ERR_STR_NONE);
	errList.AddMessage(ERR_MEM,  ERR_STR_MEM);
	errList.AddMessage(ERR_WRONGFUNC, ERR_STR_WRONGFUNC);
	errList.AddMessage(ERR_FUNCEVAL, ERR_STR_FUNCEVAL);

	a_g = a_h = a_xi = a_pt = a_gx = a_gy = 0;

	bRecordSteps = false;

	minFunc = 0;
	errList.SetMessage(ERR_WRONGFUNC);
}


////////////////////////////////////////////////////////////////////////////////
/// Constructor

MacOpt::MacOpt(MinFuncBase& _minFunc, const TMacOptAttribs &sAttrib)
{
	errList.AddMessage(ERR_NONE, ERR_STR_NONE);
	errList.AddMessage(ERR_MEM,  ERR_STR_MEM);
	errList.AddMessage(ERR_WRONGFUNC, ERR_STR_WRONGFUNC);
	errList.AddMessage(ERR_FUNCEVAL, ERR_STR_FUNCEVAL);

	a_g = a_h = a_xi = a_pt = a_gx = a_gy = 0;

	bRecordSteps = false;

	Init(_minFunc, sAttrib);
}


////////////////////////////////////////////////////////////////////////////////
/// Destructor

MacOpt::~MacOpt() 
{
	if (a_g) delete[] a_g;
	if (a_h) delete[] a_h;
	if (a_xi) delete[] a_xi;
	if (a_pt) delete[] a_pt;
	if (a_gx) delete[] a_gx;
	if (a_gy) delete[] a_gy;
}



////////////////////////////////////////////////////////////////////////////////
/// Init

bool MacOpt::Init(MinFuncBase& _minFunc, const TMacOptAttribs &sAttrib)
{
	cStr str;

	errList.ResetList();
	msgList.ResetList();

	a_verbose = sAttrib.verbose;
	a_tol = sAttrib.tolerance;
	a_itmax = sAttrib.itmax;
	a_rich = sAttrib.rich;

	a_stepmax = sAttrib.stepmax;

	if (sAttrib.linmin_g[0] && sAttrib.linmin_g[1] && sAttrib.linmin_g[2])
	{
		a_linmin_g1 = sAttrib.linmin_g[0]; 
		a_linmin_g2 = sAttrib.linmin_g[1]; 
		a_linmin_g3 = sAttrib.linmin_g[2]; 
	}
	else
	{
		// Standard Values suggested by D.MacKay
		a_linmin_g1 = 2.0;
		a_linmin_g2 = 1.25;
		a_linmin_g3 = 0.5;
	}
	
	if (_minFunc.GetMaxDerivative() < 1 || (a_n = _minFunc.GetDimension()) == 0)
	{
		errList.SetMessage(ERR_WRONGFUNC);
		minFunc = 0;
		return false;
	}
	else 
	{
		minFunc = &_minFunc;
		a_n = minFunc->GetDimension();
	}


	if (a_g) delete[] a_g;
	if (a_h) delete[] a_h;
	if (a_xi) delete[] a_xi;
	if (a_pt) delete[] a_pt;
	if (a_gx) delete[] a_gx;
	if (a_gy) delete[] a_gy;

	a_g = new double[a_n] ;
	a_h = new double[a_n] ;
	a_xi = new double[a_n] ;
	a_pt = new double[a_n] ; /* scratch vector for sole use of macprod */
	a_gx = new double[a_n] ; /* scratch gradients             */
	a_gy = new double[a_n] ; /* used by maclinmin and macprod */
							 /* if verbose = 1 then there is one report for each
							 line minimization.
							 if verbose = 2 then there is an additional 
							 report for
							 each step of the line minimization.
							 if verbose = 3 then extra debugging 
							 routines kick in.
							 */

	if (!a_g || !a_h || !a_xi || !a_pt || !a_gx || !a_gy)
	{
		errList.SetMessage(ERR_MEM);

		if (a_g) delete[] a_g;
		if (a_h) delete[] a_h;
		if (a_xi) delete[] a_xi;
		if (a_pt) delete[] a_pt;
		if (a_gx) delete[] a_gx;
		if (a_gy) delete[] a_gy;

		a_g = a_h = a_xi = a_pt = a_gx = a_gy = 0;
	
		return false;
	}


	a_end_if_small_step = 1 ; /* Change this to 0/1 if you prefer */
	//a_stepmax = 0.5 ; 
	
	a_grad_tol_tiny = 1e-16 ; /* Probably not worth fiddling with */
	a_step_tol_tiny = 0.0 ;   /* Probably not worth fiddling with */
	a_linmin_maxits = 20 ;    /* Probably not worth fiddling with */
	a_lastx = 0.01 ;          /* only has a transient effect      */
	a_lastx_default = 0.01 ;  /* -- defines typical distance in parameter
								space at which the line minimum is expected;
								both these should be set. the default is 
								consulted if something goes badly wrong and 
							    a reset is demanded. */
							  
	/* don't fiddle with the following, unless you really mean it */
	/*
	a_linmin_g1 = 1.5;//2.0 ; 
	a_linmin_g2 = 1.25 ; 
	a_linmin_g3 = 0.5 ; 
	*/
	a_restart = 0 ; 

	return true;
}



//////////////////////////////////////////////////////////////////////////////////////
/// Set FuncEval Error

void MacOpt::SetFuncEvalError(uint deriv, double *p)
{
	int j;
	cStr str;

	str = errList.GetMessageData(ERR_FUNCEVAL);
	str << " " << deriv << ": (";

	for(j=0;j<a_n;j++)
	{
		str << p[j];
		if (j+1 < a_n) str << ", ";
	}
	str << ")";

	errList.SetMessage(ERR_FUNCEVAL, str.Str());
}


//////////////////////////////////////////////////////////////////////////////////////
/// macoptII
//////////////////////////////////////////////////////////////////////////////////////

// p:	starting vector

bool MacOpt::MacOptII( double *p )                   
{
	int i, n;
	int j ;
	double gg , gam , dgg ;
	double *g , *h , *xi ;
	int end_if_small_grad = 1 - a_end_if_small_step ;
	double step , tmpd ;
	char hstr[200];
	cStr str;
	
	// Check for general error in class
	if (errList.HasMessage()) return false;

	// Reset Step Record
	mStepRec = 0;

	/* A total of 7 double * 1..n are used by this optimizer. 
	p           is provided when the optimizer is called 
	pt          is used by the line minimizer as the temporary vector. 
	this could be cut out with minor rewriting, using p alone
	g, h and xi are used by the cg method as in NR - could one of these
	be cut out?
	the line minimizer uses an extra gx and gy to evaluate two gradients. 
	*/
	
	g = a_g ; 
	h = a_h ;
	xi = a_xi ;
	
	if (bRecordSteps)
	{
		n = mStepRec.Count();
		if (mStepRec += a_n)
		{
			for(i=0;i<a_n;i++)
			{
				mStepRec[n+i] = p[i];
			}
		}
	}
	
	if (!minFunc->Func( 1 /*1st Deriv.*/, p , xi )) // Derivative Call
	{
		SetFuncEvalError(1, p);
		return false; // Error in function evaluation
	}
	
	MacOptRestart ( 1 ) ; 
	
	for ( a_its = 0 ; a_its < a_itmax ; a_its ++ ) 
	{
		for ( gg = 0.0 , j = 0 ; j < a_n ; j ++ ) 
			gg += g[j]*g[j];          /* find the magnitude of the old gradient */
	
		a_gtyp = sqrt ( gg / (double)(a_n) ) ; 
		
		if ( a_verbose > 0 ) 
		{
			sprintf(hstr, "mac_it %d of %d : gg = %6.3g tol = %6.3g: ", a_its , a_itmax , gg , a_tol );
			msgList.SetMessage(MSG_VB1, hstr);
		}
		
		if ( ( end_if_small_grad && ( gg <= a_tol ) ) 
			|| ( gg <= a_grad_tol_tiny ) ) 
		{
			return true;
		}
		
		step = MacLinMinII ( p  ) ; 
		if (errList.HasMessage()) return false;

		if (bRecordSteps)
		{
			n = mStepRec.Count();
			if (mStepRec += a_n)
			{
				for(i=0;i<a_n;i++)
				{
					mStepRec[n+i] = p[i];
				}
			}
		}

		if ( a_restart == 0 ) 
		{
			if ( a_verbose > 1 ) 
			{
				sprintf(hstr, "(step %9.5g)",step);
				msgList.SetMessage(MSG_VB2, hstr);
			}

			if ( ( a_end_if_small_step  && ( step <= a_tol ) ) 
				|| ( step <= a_step_tol_tiny ) ) 
			{
				return true;
			}
		}
		
		/* if we are feeling rich, evaluate the gradient at the new
		`minimum'. alternatively, linmin has already estimated this
		gradient by linear combination of the last two evaluations and
		left it in xi */
		if ( a_rich || a_restart ) 
		{ 
			if (!minFunc->Func( 1/* 1st Deriv. */, p , xi  )) // Derivative Call
			{
				SetFuncEvalError(1, p);
			}
		}
		
		if ( a_restart ) 
		{
			if(a_verbose > 0)
			{
				msgList.SetMessage(MSG_VB1, "Restarting MacOpt."); 
			}

			MacOptRestart ( 0 ) ;
			/* this is not quite right
			should distinguish whether there was an overrun indicating that the 
			value of lastx needs to be bigger / smaller; 
			in which case resetting lastx to default value may be a bad idea, 
			giving an endless loop of resets 
			*/
		} 
		else 
		{
			dgg=0.0;
			for ( j = 0 ; j < a_n ; j ++ ) 
			{
				dgg += ( xi[j] + g[j] ) * xi[j] ;
			}
			
			gam = dgg / gg ;
			
			for ( tmpd = 0.0 , j = 0 ; j < a_n ; j ++ ) 
			{
				g[j] = -xi[j];                /* g stores (-) the most recent gradient */
				xi[j] = h[j] = g[j] + gam * h[j] ;
				/* h stores xi, the current line direction */
				/* check that the inner product of gradient and line search is < 0 */
				tmpd -= xi[j] * g[j] ; 
			}
			
			if ( tmpd > 0.0  || a_verbose > 2 ) 
			{
				sprintf(hstr,"New line search has inner prod %9.4g", tmpd ); 
				msgList.SetMessage(MSG_VB3, hstr);
			}
			
			if ( tmpd > 0.0 ) 
			{ 
				if ( !a_rich ) 
				{
					if (a_verbose > 0)
					{
						msgList.SetMessage(MSG_VB1, "Setting rich to 1.");
					}
					a_rich = true ; 
				}
				
				a_restart = 2 ; /* signifies that g[j] = -xi[j] is already done */
				
				if (a_verbose > 0)
				{
					msgList.SetMessage(MSG_VB1, "Restarting macopt (2)");
				}
			
				MacOptRestart ( 0 ) ;
			}
		}
	}

	msgList.SetMessage(MSG_WARN, "Reached iteration limit in macopt; continuing.");
	
	return true;
	
} /* NB this leaves the best value of p in the p vector, but
	the function has not been evaluated there if rich=0     */


///////////////////////////////////////////////////////////////////////////////////
/// Check Step Size
/// Added by C.Perwass 10.10.99
///////////////////////////////////////////////////////////////////////////////////

double MacOpt::AdjustStepSize(double x)
{
	int i, imax = 0;
	double val, max = 0.0;

	for(i=0;i<a_n;i++)
	{
		if ((val = Mag(x * a_xi[i])) > max)
		{
			max = val;
			imax = i;
		}
	}

	if (max > a_stepmax) x = Mag(a_stepmax/a_xi[imax]);

	return x;
}


///////////////////////////////////////////////////////////////////////////////////
/// maxlinminII
///////////////////////////////////////////////////////////////////////////////////

/* 
maclinmin.

Method: 
	evaluate gradient at a sequence of points and calculate the inner 
	product with the line search direction. Continue until a 
	bracketing is achieved ( i.e a change in sign ). 

*/

double MacOpt::MacLinMinII(double *p)
{
	// Check for general error in class
	if (errList.HasMessage()) return 0.0;
	
	int n = a_n ; 
	
	double x , y ;
	double s , t , m ;
	int    its = 0 , i ;
	double step , tmpd ; 
	double  *gx = a_gx , *gy = a_gy ;
	char hstr[200];
	

	
	/* at x=0, the gradient (uphill) satisfies s < 0 */
	if ( a_verbose > 2 ) 
	{ /* check this is true: (no need to do this really
	  as it is already checked at the end of the main
		loop of macopt) */
		/*
		#define TESTS 5
		x = a_lastx / a_gtyp ;
		fprintf (stderr, "inner product at:\n" ) ; 
		for ( i = -TESTS ; i <= TESTS ; i += 2 ) {
		step = x * 2.0 * (double) i / (double) TESTS ; 
		fprintf (stderr, "%9.5g %9.5g\n" , step ,
		tmpd = macprodII ( p , gy , step ) ) ; 
		}
		*/
		sprintf (hstr, "inner product at 0 = %9.4g", tmpd = MacProdII ( p , gy , 0.0 ) ) ; 
		msgList.SetMessage(MSG_VB3, hstr);

		if ( tmpd > 0.0 ) 
		{ 
			a_restart = 1 ; 
			return 0.0 ; 
		}
	}
	
	x = AdjustStepSize(a_lastx / a_gtyp);
	s = MacProdII ( p , gx , x ) ; 
	if (errList.HasMessage()) return 0.0;

	if ( s < 0 )  
	{  /* we need to go further */
		do 
		{
			y = AdjustStepSize(x * a_linmin_g1);
			
			t = MacProdII ( p , gy , y  ) ; 
			if (errList.HasMessage()) return 0.0;
		
			if ( a_verbose > 1 ) 
			{
				sprintf (hstr, "s = %6.3g: t = %6.3g; x = %6.3g y = %6.3g",s, t , x , y );
				msgList.SetMessage(MSG_VB2, hstr);
			}

			if ( t >= 0.0 ) break ;
			x = y ; s = t ; a_gunused = gx ; gx = gy ; gy = a_gunused ; 
			its++ ;
			/* replaces: for ( i = 1 ; i <= n ; i ++ ) gx[i] = gy[i] ; */
		}
		while ( its < a_linmin_maxits ) ;
	} 
	else if ( s > 0 ) 
	{ /* need to step back inside interval */
		do 
		{
			y = AdjustStepSize(x * a_linmin_g3);
		
			t = MacProdII ( p , gy , y ) ; 
			if (errList.HasMessage()) return 0.0;

			if ( a_verbose > 1 ) 
			{
				sprintf (hstr, "s = %6.3g: t = %6.3g; x = %6.3g y = %6.3g",s, t , x , y );
				msgList.SetMessage(MSG_VB2, hstr);
			}

			if ( t <= 0.0 ) break ;

			x = y ; s = t ; a_gunused = gx ; gx = gy ; gy = a_gunused ; 
			its ++ ;
		} while ( its < a_linmin_maxits ) ;
	} 
	else 
	{ /* hole in one s = 0.0 */
		t = 1.0 ; y = x;
	}
	
	if ( its >= a_linmin_maxits )  
	{
		msgList.SetMessage(MSG_WARN, "Warning! MacLinMin overran");
		/* 
		this can happen where the function goes \_ and doesn't buck up
		again; it also happens if the initial `gradient' does not satisfy
		gradient.`gradient' > 0, so that there is no minimum in the supposed
		downhill direction.  I don't know if this actually happens... If it
		does then I guess a_rich should be 1.
		
		If the overrun is because too big a step was taken then
		the interpolation should be made between zero and the most 
		recent measurement. 
		  
		If the overrun is because too small a step was taken then 
		the best place to go is the most distant point. 
		I will assume that this doesn't happen for the moment.
			
		Also need to check up what happens to t and s in the case of overrun.
		And gx and gy. 
			  
		Maybe sort this out when writing a macopt that makes use of the gradient 
		at zero? 
		*/
		
		sprintf(hstr, "- inner product at 0 = %9.4g" ,
			tmpd = MacProdII ( p , gy , 0.0 ) ) ; 
		msgList.SetMessage(MSG_WARN, hstr);
		
		if ( tmpd > 0 && !a_rich ) 
		{
			msgList.SetMessage(MSG_WARN, "Setting rich to 1.");
			a_rich = true; 
		}
		
		if ( tmpd > 0 ) a_restart = 1 ; 
	}
	
	/*  Linear interpolate between the last two. 
	This assumes that x and y do bracket. */
	if ( s < 0.0 ) s = - s ;
	if ( t < 0.0 ) t = - t ;
	m = ( s + t ) ;
	s /= m ; t /= m ;
	
	m =  s * y + t * x ; 
	/* evaluate the step length, not that it necessarily means anything */
	for ( step = 0.0 , i = 0; i < n ; i ++ ) 
	{
		tmpd = m * a_xi[i] ;
		p[i] += tmpd ; /* this is the point where the parameter vector steps */
		step += fabs ( tmpd ) ; 
		a_xi[i] = s * gy[i] + t * gx[i] ;
		/* send back the estimated gradient in xi (NB not like linmin) */
	}
	a_lastx = m * a_linmin_g2 *  a_gtyp ;
	
	return ( step / (double) ( n ) ) ; 
}



///////////////////////////////////////////////////////////////////////////////////
/// macprodII
///////////////////////////////////////////////////////////////////////////////////


double MacOpt::MacProdII(double *p , double *gy , double y) 
{
	// Check for general error in class
	if (errList.HasMessage()) return 0.0;

	double *pt = a_pt ; 
	double *xi = a_xi ; 
	/* finds pt = p + y xi and gets gy there, 
	returning gy . xi */
	int n = a_n ; 
	
	int i;
	double s = 0.0 ;
	
	for ( i = 0 ; i < n ; i ++ ) 
		pt[i] = p[i] + y * xi[i] ;
	
	if (!minFunc->Func(1 /* 1st Deriv. */, pt , gy )) /// Derivative Call
	{
		SetFuncEvalError(1, pt);
		return 0.0;
	}
	
	for ( i = 0 ; i < n ; i ++ ) 
		s += gy[i] * xi[i] ;
	
	return s ;
}


///////////////////////////////////////////////////////////////////////////////////
/// macopt_restart
///////////////////////////////////////////////////////////////////////////////////


void MacOpt::MacOptRestart ( int start ) 
/* if start == 1 then this is the start of a fresh macopt, not a restart */
{
	// Check for general error in class
	if (errList.HasMessage()) return;

	int j , n = a_n ; 
	double *g, *h, *xi ;
	g = a_g ;  h = a_h ;  xi = a_xi ; 
	
	if ( start == 0 ) a_lastx = a_lastx_default ; 
	
	/* it is assumed that dfunc( p , xi  ) ;  has happened */
	for ( j = 0 ; j < n ; j ++ ) 
	{
		if ( a_restart != 2 ) g[j] = -xi[j] ;
		xi[j] = h[j] = g[j] ;
	}
	a_restart = 0 ; 
}


///////////////////////////////////////////////////////////////////////////////////
/// maccheckgrad
///////////////////////////////////////////////////////////////////////////////////


bool MacOpt::MacCheckGrad(double *p, double epsilon, 
						  int stopat /* stop at this component. If 0, do the lot. */)

/* Examines objective function and d_objective function to see if 
they agree for a step of size epsilon */
{
	// Check for general error in class
	if (errList.HasMessage()) return false;
	
	int j, n = a_n;
	double f1, f2;
	double *g,*h;
	double tmpp ; 
	char hstr[200];
	
	if (!minFunc->Func(0/* the function */, p, &f1))
	{
		SetFuncEvalError(0, p);
		return false; // Error in function evaluation
	}

	//  printf("f1 = %f\n", f1);
	
	h = new double[n];
	g = new double[n];

	if (!h || ! g)
	{
		errList.SetMessage(ERR_MEM);

		if (h) delete[] h;
		if (g) delete[] g;

		return false;
	}
	
	if (!minFunc->Func(1/* 1st Deriv. */, p, g)) // Dervative Call
	{
		SetFuncEvalError(1, p);
		return false; // Error in deriv. evaluation
	}
	
	if ( stopat <= 0 || stopat > n ) stopat = n ; 
	
	msgList.SetMessage(MSG_CHECK, "Testing gradient evaluation");
	msgList.SetMessage(MSG_CHECK, "      analytic     1st_diffs    difference");
	for ( j = 0 ; j < stopat ; j ++ ) 
	{
		tmpp = p[j] ; 
		p[j] += epsilon ;

		if (!minFunc->Func(0, p, &f2))
		{
			SetFuncEvalError(0, p);
			return false;
		}

		h[j] = f2 - f1 ;
		//    printf("h = %f\n", h[j]);
		p[j] =  tmpp ;
		
		sprintf(hstr, "%2d %9.5g %9.5g %9.5g" , j , g[j] , h[j]/epsilon , g[j] - h[j]/epsilon );
		msgList.SetMessage(MSG_CHECK, hstr);
	}

	delete[] h;
	delete[] g;

	msgList.SetMessage(MSG_CHECK, "      --------     ---------");

	return true;
}

