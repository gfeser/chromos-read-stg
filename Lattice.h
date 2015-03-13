// Lattice.h: interface for the CLattice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LATTICE_H__A30E8576_CF48_11D2_B791_343579000000__INCLUDED_)
#define AFX_LATTICE_H__A30E8576_CF48_11D2_B791_343579000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"
#include <math.h>
#define LATTICE_T float

// CLattice	представляет профиль (функцию), заданную в наборе точек (узлов)
// с постоянным	шагом.
// Умеет находить значение функции в произвольной точке.

class CLattice : public CArray<LATTICE_T, LATTICE_T>  
{
public:
	void GetLinearRegression( double &k, double &b) const;
	double GetArea( double t1, double t2) const;
	CLattice( double dt, double t0);
	CLattice();
	virtual ~CLattice();

	void SetLattice( double dt, double t0)  { m_t0=t0;  m_dt=dt;}
	double GetDt() const { return m_dt;}
	double GetT0() const { return m_t0;}
	double GetTime( int n) const { return m_t0+m_dt*n;}
	double GetData( double t) const;
	LATTICE_T* GetData()  { return CArray<LATTICE_T, LATTICE_T>::GetData();}
	const LATTICE_T* GetData() const { return CArray<LATTICE_T, LATTICE_T>::GetData();}
	LATTICE_T operator[]( double t) const { return (LATTICE_T)GetData(t);}
	LATTICE_T operator[]( int n) const { return CArray<LATTICE_T, LATTICE_T>::GetAt(n);}
	LATTICE_T& operator[]( int n) { return CArray<LATTICE_T, LATTICE_T>::ElementAt(n);}

	static double approx( double t, double t1, double t2, double y1, double y2)
		{ 
		if(t2==t1) return y1;
		return y1+(t-t1)*(y2-y1)/(t2-t1); }

private:
	double m_t0; // время первой выборки
	double m_dt; // время между выборками

};


#endif // !defined(AFX_LATTICE_H__A30E8576_CF48_11D2_B791_343579000000__INCLUDED_)
