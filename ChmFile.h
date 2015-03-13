// ChmFile.h: interface for the CChmFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHMFILE_H__1FBA45C1_1BC7_4B16_AEB8_B74F43C8D147__INCLUDED_)
#define AFX_CHMFILE_H__1FBA45C1_1BC7_4B16_AEB8_B74F43C8D147__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Cmpeeks.h"

class CChmFile  
{
public:
	BOOL ReadChmFile(LPCTSTR lpszPathName);
	void TraceAreaData();

	CChmFile();
	virtual ~CChmFile();

	CLattice m_fData;
	CString m_FullPathName;
	int    m_DataLength;
	double  m_dt;
	double m_dt0;	// initial values, without fiducial peaks correction

//	CPassport ps;
	CPeeksTable peeks;

//	CChmFile( const CChmFile& info1)  { *this=info1;}
//	const CChmFile& operator=( const CChmFile& info1); 
};

#endif // !defined(AFX_CHMFILE_H__1FBA45C1_1BC7_4B16_AEB8_B74F43C8D147__INCLUDED_)
