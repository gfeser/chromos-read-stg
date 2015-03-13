// Lattice.cpp: implementation of the CLattice class.
//
//////////////////////////////////////////////////////////////////////

#include "Lattice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const double TOLERANCE = 1e-100;		  // Tolerance of Zero

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLattice::CLattice()
{
	m_t0=0;  m_dt=1;
}

CLattice::CLattice( double dt, double t0)
{
	SetLattice( dt, t0);
}

CLattice::~CLattice()
{

}

double CLattice::GetData(double t) const
{
	ASSERT(GetSize()>=2);
	int n=int(floor((t-m_t0)/m_dt));
	if( n<0) return approx( t, GetTime(0), GetTime(1), GetAt(0), GetAt(1));
	if( n>=GetUpperBound()) 
		return approx( t, GetTime(GetUpperBound()-1), GetTime(GetUpperBound()),
						GetAt(GetUpperBound()-1), GetAt(GetUpperBound()));

	return approx( t, GetTime(n), GetTime(n+1),	GetAt(n), GetAt(n+1));
}

double CLattice::GetArea(double t1, double t2) const
{
	return 0;
}

void CLattice::GetLinearRegression(double &k, double &b) const
{
	int N=GetSize();
	if( N==0) {
		k=0; b=0;
		return;
	}

	double sx=0, sy=0, sx2=0, sxy=0;
	for( int i=0; i<N; i++)  {
		double x=GetTime(i), y=GetAt(i);
		sx+=x; sy+=y, sxy+=x*y; sx2+=x*x;
	}

	if( N==1)	 {
		k = (fabs(sx)<TOLERANCE)? 0 : sy/sx;
		b = 0;
		return;
	}

	double d=(N*sx2-sx*sx);
	if( fabs(d)<TOLERANCE)	{
		k=0; b=0;
		return;
	} else  {
		k = (N*sxy-sx*sy)/d;
		b = (sy - k*sx)/N;
	}
	
}
