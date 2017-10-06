// cmpeeks.h
//

#ifndef CMPEEKS_H
#define CMPEEKS_H

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <afxole.h>
#include <afxpriv.h>
#include <afxdisp.h> // CG: added by ActiveX Control Containment component
#include <afxmt.h>

#include <atlbase.h>

#include <math.h>
#include <float.h>
#include <iterator>
#include <algorithm>
#include <vector>
#include <map>


#include "Lattice.h"

#define DOP_PAR_INDEXES "»ндекс"



using namespace std;

class CPeeksTable;

// cmpeek - один пик в таблице пиков хроматограммы

class cmpeek {
friend class CPeeksTable;

public:


class TwinsDopParam {

public:
	CString name;
	CString value;

public:
	TwinsDopParam() {name="noname"; value="novalue";}
	TwinsDopParam(const TwinsDopParam&  twdp) { name=twdp.name; value=twdp.value; }
	TwinsDopParam(CString& name1, CString& value1) { name=name1; value=value1; }
	virtual ~TwinsDopParam() {};

	TwinsDopParam& operator=(const TwinsDopParam& twdp){ name=twdp.name; value=twdp.value; return (*this); }

	};



class DopParamList : public vector <TwinsDopParam> {

public:
	DopParamList() { }
	~DopParamList() { clear(); }
	
	void Serialize(CArchive &ar, int ver);
	void    SetParam(CString & name, CString& value);
	CString GetParam(CString &name);
	void    ClearParam(CString &name);
	const DopParamList& operator=( const DopParamList& list1 ) ;
	BOOL IsParamExist(CString &name);
	BOOL    IsDopParamExist() { return ( size()>0); }

	};

	enum EDGES { NOEDGE, FRONT, BACK, AUTOMARK, AUTOMARK_ENDGROUP};
	enum ATTRIB { ORDINARY, RIDER, LEFT_STUCK, RIGHT_STUCK};
	enum PeekType {
		PT_Normal=0,
		PT_Rider=1,
		PT_Group=2 };

public://было protected
	double m_time;			// врем€ выхода
	double m_area;			// площадь
	double m_height;		// высота пика
	double m_concent;		// концентраци€
	CString m_comment;		// название компонента
	int m_number;

	BOOL m_fiducial;		// реперный
	BOOL m_rider;			// наездник
	BOOL m_begingroup;		// начало группы

	int m_left;				// номер неразделенного пика слева, или (-1)
	int m_right;			// 

	double m_back_height;
	double m_front_height;
	double m_front;			// врем€ начала пика
	double m_back;
	
	DopParamList m_DopParams; // список дополнительных параметров

	double m_temp_index; // временное хранение индексов, не в виде строки, а в виде double дл€ функции индентификации

protected:
	struct PEEK_BUFF { // дл€ записи в файл хроматограммы
	protected:
		enum {COMM_LENGTH=32};
	public:
		double back;
		double front;
		double concent;
		char comment[COMM_LENGTH];
		int right;
		__int8 reserved[48];
	};

public:
	double GetRetentionIndex();
	void CheckRider(double hRider);
//	int Found(int posp);
//	void FindedExt(int i,CLattice m_buf);
    cmpeek(double f, double b, int number, int left=-1, int right=-1);
    cmpeek();
    cmpeek( const cmpeek& peek1)  { *this=peek1;}
    ~cmpeek() {};
	
	void Serialize(CArchive & ar, int ver, int number=-1);
	void SetProperties( const CLattice &data); 

	void    SetDopParam(CString &name, CString &value) { m_DopParams.SetParam(name,value); }
	CString GetDopParam(CString &name) { return m_DopParams.GetParam(name); }
	void    ClearDopParam(CString &name) { m_DopParams.ClearParam(name); }
	BOOL    IsDopParamExist() { return m_DopParams.IsDopParamExist(); }


//	const LPCTSTR GetComment() const { return (LPCTSTR)m_comment; }
	const CString& GetComment() const { return m_comment; }
	void SetComment(LPCTSTR str) { m_comment = str; }

	double GetConcent() const { return m_concent; }
	void SetConcent(double h) { m_concent = h; }
	
	void SetFront(double f) { m_front = f; }
	void SetBack(double b) { m_back = b; }
	double GetFront() const { return m_front; }
	double GetBack() const { return m_back; }

	double GetWidth() const { return m_back-m_front; }

	int GetLeft() const { return m_left; }
	int GetRight() const { return m_right; }
	int GetNumber()	const { return m_number;}

	double GetFrontHeight() const { return m_front_height; }
	double GetBackHeight() const { return m_back_height; }

	double GetHeight() const { return m_height; }
	double GetTime() const { return m_time; }	
	double GetArea() const { return m_area; }
	
	BOOL GetFiducial() const { return m_fiducial;}
	void SetFiducial( BOOL fiducial)  { m_fiducial=fiducial;}
	
	BOOL IsFound(double t1, double t2);
	BOOL IsFound( int number)   { return number==m_number;}
	EDGES Edge(double t1, double t2);

	BOOL IsRider()						{ return (m_rider&PT_Rider); }
	BOOL IsGroupFlag()					{ return (m_rider&PT_Group); }
	void SetBeginGroup(BOOL beg=TRUE)	{ m_begingroup=beg; }
	BOOL IsBeginGroup()					{ return m_begingroup; }

	bool operator<( const cmpeek &peek1) const { return m_time<peek1.m_time; }
	bool operator==( const cmpeek &peek1) const { return m_time==peek1.m_time; }
	const cmpeek& operator=( const cmpeek &peek1);
	//CString GetComponentDataStr();

	
protected:
	void SetLeft( int left)  { m_left=left; }
	void SetRight( int right)  { m_right=right; }
	void SetNumber( int number)	{ m_number=number;}
//	void SetBaseLine( double t1, double t2, const CArray<float, float> &data, 
//		double dt, double t0);
	void SetBaseLine( double t1, double t2, const CLattice &data);
	void SubsArea( double a1)  { m_area-=a1; }
};

// CPeeksTable - таблица пиков хроматограммы

class CPeeksTable : public vector<cmpeek> 
{
public:
	enum {SIGNATURE1=0x125FFE11, SIGNATURE2=0xA2D28C45};

	// ver 2 mainstream
	// ver 3 add dop.parameters (beta) for load VERYYYY BIG files
	// ver 4 mainstream

	enum {VERSION=4}; 

	enum FindType {
		FT_Begin=0,
		FT_Front,
		FT_Plato,
		FT_Back };


	void CorrectTime( double k_corr, double t0);
	const double CPeeksTable::POINTS_PER_PEEK;

	
	void Reorganize();
	CPeeksTable();
	virtual ~CPeeksTable();
	void Read( CFile &file, int stream_size /*used for old style*/);
	void Read( LPSTREAM pStream);
	void Save( CFile &file);
	void Save( LPSTREAM pStream);
//	void SetData( const CArray<float, float> &fData, double dt, double t0);  
	void SetData( const CLattice &fData)  { m_fData = &fData;}  

	int FindPeek( LPCTSTR str);
	void DeletePeek(int n, bool UpdatePeeks=true, bool RealDel=false);
	bool AddPeek(double f, double b, bool Reorganize=true, int type=cmpeek::PT_Normal);
	bool AddStuckPeek(int basenum, double thisTime, cmpeek::EDGES edge);
	bool ModifyPeek( int num, double f, double b);
	BOOL  IsDopParamExist();

	float  ExpansionPeek(int& i_left, int& i_right);
	float CalcSimplePeekArea(int i_left, int i_right);
static	float CalcSimplePeekAreaBuff(int i_left, int i_right, const float* buff, int N, float dt );
void MakeGroups();
void CheckAndMarkGroup(int start_number, int stop_number);
BOOL IsPossibleAutomartOnEvent(double time1,double time2);


	void UpdatePeeks();  // Recalculates height and area of each peek
	void UpdateLinks();  // Recovers links of stacked peeks after sorting

	const CPeeksTable& operator=( const CPeeksTable &table1);

//	int FindedExtr(int i, bool rostflag);

public://было private

	const CLattice *m_fData;

//	CLattice m_buf;
//	CLattice m_BaseLine; //базова€ лини€ дл€ авторазметки
	CArray<double, double> m_maximas;
	int m_nPeekStart, m_nGroupStart;
//	bool m_bAtBack;
//	bool m_bAtFront;
	int		m_FindType;

	double 	m_peekThresh, m_groupThresh, m_Assim;
	double m_minHeight,  m_minArea,  m_minTime,  m_minWidth;
	double m_noise;
	double m_riderHeight;
	
	cmpeek::DopParamList m_CommonDopParams; // список дополнительных параметров


protected:
	void MarkRiders();
	void CheckAndAddPeek(int i);
//	void PeekMarkStep( int i);
// мое
public:
	CString GetPeekTableStr();
	void LoadDopParamNames(cmpeek::DopParamList* list);
	void CorrectBaseLine(cmpeek& peek, BOOL isGroup);
	void KorrectMark();
	double GetMaxHeight();
	int Found(int posp);
//	void DelBaseLine();
	void ExpandLastGroupPeek( double thisTime);
	class CompEvent
	{
	public:
		int EventNum;
		double EventStart;
		double EventParam;
	};
	CMap <int,int,CompEvent,CompEvent> EList;
};
/*
inline void CPeeksTable::SetData( const CArray<float, float> &fData, 
								 double dt, double t0)
{
	m_fData = &fData;   m_dt=dt;   m_t0=t0;
}
*/
#endif // CMPEEKS_H