// ChmFile.cpp: implementation of the CChmFile class.
//
//////////////////////////////////////////////////////////////////////

#include "cmpeeks.h"
#include "Lattice.h"
#include "ChmFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CChmFile::CChmFile()
{
	m_FullPathName = "";
	m_DataLength=0;
}

CChmFile::~CChmFile()
{

}

BOOL CChmFile::ReadChmFile(LPCTSTR lpszPathName)
{
    USES_CONVERSION;
    LPSTORAGE pStgRoot = NULL;
    LPSTREAM pStream = NULL;
    ULONG nLength;
    int nDataLength = 0;

	if( ::StgOpenStorage(T2COLE(lpszPathName), NULL,
					STGM_READ | STGM_SHARE_EXCLUSIVE,
					NULL, 0, &pStgRoot) != S_OK)
		return FALSE;
	ASSERT(pStgRoot!= NULL);

	BOOL ret=TRUE;
	char szStreamName[100] = "Raw_data";

    try {

// read data
		if( pStgRoot->OpenStream(T2COLE(szStreamName), NULL,
						STGM_READ | STGM_SHARE_EXCLUSIVE,
						0, &pStream) != S_OK)
			AfxThrowArchiveException( CArchiveException::badIndex, lpszPathName);

		ASSERT(pStream != NULL);

		ULARGE_INTEGER libNewPosition = {0, 0};
		LARGE_INTEGER startPosition = {0, 0};
		VERIFY(pStream->Seek(startPosition, STREAM_SEEK_END, &libNewPosition) == S_OK);
		nDataLength = libNewPosition.LowPart / sizeof(m_fData[0]);
		VERIFY(pStream->Seek(startPosition, STREAM_SEEK_SET, NULL) == S_OK);

// allocate arrays    
		m_fData.SetSize(nDataLength);

		VERIFY(pStream->Read(m_fData.GetData(), 
			libNewPosition.LowPart , &nLength) == S_OK);
		pStream->Release();

// read time
		pStream = NULL;
		strcpy(szStreamName, "Time");
		szStreamName[31] = '\0';
		if( pStgRoot->OpenStream(T2COLE(szStreamName), NULL,
								STGM_READ | STGM_SHARE_EXCLUSIVE,
								0, &pStream) != S_OK)
			AfxThrowArchiveException( CArchiveException::badIndex, lpszPathName);

		ASSERT(pStream != NULL);

		libNewPosition.LowPart = 0;
		libNewPosition.HighPart = 0;
    
		VERIFY(pStream->Seek(startPosition, STREAM_SEEK_END, &libNewPosition) == S_OK);

		float Time[3];
		if ( libNewPosition.LowPart < sizeof(Time)) {
			m_fData.SetLattice( 1, 0);
			m_dt0=1;
		} else {
			VERIFY(pStream->Seek(startPosition, STREAM_SEEK_SET, NULL) == S_OK);
			VERIFY(pStream->Read( Time, sizeof(Time), &nLength) == S_OK);
			m_fData.SetLattice( Time[1]-Time[0], Time[0]);
			m_dt0 = Time[2]-Time[1];
		}
		pStream->Release();

// read chanell 2
//
// read peaks table

		peeks.SetData( m_fData);
		pStream = NULL;
		strcpy(szStreamName, "Table");
		if(pStgRoot->OpenStream(T2COLE(szStreamName), NULL,
							STGM_READ | STGM_SHARE_EXCLUSIVE,
							0, &pStream) != S_OK)
			AfxThrowArchiveException( CArchiveException::badIndex, lpszPathName);

		ASSERT(pStream != NULL);
		peeks.Read( pStream);
		pStream->Release();

// read passport
/*		pStream = NULL;
		strcpy(szStreamName, CPassport::defaultStream);
		if (pStgRoot->OpenStream(T2COLE(szStreamName), NULL,
								STGM_READ | STGM_SHARE_EXCLUSIVE,
								0, &pStream) != S_OK) 
			AfxThrowArchiveException( CArchiveException::badIndex, lpszPathName);

		ASSERT(pStream != NULL);
		ps.Read( pStream, lpszPathName);

		pStream->Release();
*/
   	}  catch(...)  {
		ret=FALSE;
		pStream->Release();
	}

	pStgRoot->Release();
	return ret;
/*
    USES_CONVERSION;
    LPSTORAGE pStgRoot = NULL;
    LPSTREAM pStream = NULL;
    ULONG nLength;
    int nDataLength = 0;

	if( ::StgOpenStorage(T2COLE(lpszPathName), NULL,
		STGM_READ | STGM_SHARE_EXCLUSIVE,
		NULL, 0, &pStgRoot) != S_OK)
		return FALSE;

	ASSERT(pStgRoot!= NULL);

	BOOL ret=TRUE;
	char szStreamName[100] = "Raw_data";

    try {

		// read data
		if( pStgRoot->OpenStream(T2COLE(szStreamName), NULL,
						STGM_READ | STGM_SHARE_EXCLUSIVE,
						0, &pStream) != S_OK)
			AfxThrowArchiveException( CArchiveException::badIndex, lpszPathName);

		ASSERT(pStream != NULL);

		ULARGE_INTEGER libNewPosition = {0, 0};
		LARGE_INTEGER startPosition = {0, 0};
		VERIFY(pStream->Seek(startPosition, STREAM_SEEK_END, &libNewPosition) == S_OK);
		nDataLength = libNewPosition.LowPart / sizeof(float);
		VERIFY(pStream->Seek(startPosition, STREAM_SEEK_SET, NULL) == S_OK);

		// allocate arrays    
		m_fData.SetSize(nDataLength);

		VERIFY(pStream->Read(m_fData.GetData(), 
			libNewPosition.LowPart , &nLength) == S_OK);
		pStream->Release();

		// read time
		pStream = NULL;
		strcpy(szStreamName, "Time");
		szStreamName[31] = '\0';
		if( pStgRoot->OpenStream(T2COLE(szStreamName), NULL,
								STGM_READ | STGM_SHARE_EXCLUSIVE,
								0, &pStream) != S_OK)
			AfxThrowArchiveException( CArchiveException::badIndex, lpszPathName);

		ASSERT(pStream != NULL);

		libNewPosition.LowPart = 0;
		libNewPosition.HighPart = 0;
    
		VERIFY(pStream->Seek(startPosition, STREAM_SEEK_END, &libNewPosition) == S_OK);

		float Time[3];
		if ( libNewPosition.LowPart > sizeof(float)) {
			VERIFY(pStream->Seek(startPosition, STREAM_SEEK_SET, NULL) == S_OK);
			VERIFY(pStream->Read( Time, sizeof(Time), &nLength) == S_OK);
			m_dt = Time[2]-Time[1];
		}
		pStream->Release();

		// read peaks table
		peeks.SetData( m_fData);
		pStream = NULL;
		strcpy(szStreamName, "Table");
		if(pStgRoot->OpenStream(T2COLE(szStreamName), NULL,
							STGM_READ | STGM_SHARE_EXCLUSIVE,
							0, &pStream) != S_OK)
			AfxThrowArchiveException( CArchiveException::badIndex, lpszPathName);

		ASSERT(pStream != NULL);
		peeks.Read( pStream);
		pStream->Release();

		// read passport
		pStream = NULL;
		strcpy(szStreamName, CPassport::defaultStream);
		if (pStgRoot->OpenStream(T2COLE(szStreamName), NULL,
								STGM_READ | STGM_SHARE_EXCLUSIVE,
								0, &pStream) != S_OK) 
			AfxThrowArchiveException( CArchiveException::badIndex, lpszPathName);

		ASSERT(pStream != NULL);
		ps.Read( pStream, lpszPathName);
		pStream->Release();

   	}  catch(...)  {
		ret=FALSE;
		pStream->Release();
	}

	pStgRoot->Release();

	if( m_DataLength>10 && m_fData[10]<m_fData[0]) {
		double temp0=m_fData[0];
		for( int i=0; i<m_DataLength; i++) 
			m_fData[i]=temp0-m_fData[i];
	}

	m_FullPathName = lpszPathName;

	return ret;*/
}

void CChmFile::TraceAreaData()
{
	vector<cmpeek>::iterator it;

CString out;
for( it=peeks.begin(); it!=peeks.end(); it++)  {
	
	out+=it->GetComment();//имя компонента
	out+="   ";

	CString area_str; 
	area_str.Format("%.3f",it->GetArea());//площадь

	out+=area_str;
	out+="\r\n";
	}

	TRACE( out);
}