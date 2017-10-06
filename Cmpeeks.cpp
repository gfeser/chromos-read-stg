// cmpeeks.cpp
//

//m_fData - оригинальный набор точек, m_buf - вспомогательный
#include "stdafx.h"
#include "Cmpeeks.h"
cmpeek::cmpeek()
{ 
    m_front = m_back = 0;
    m_front_height = m_back_height = 0;
	m_area=m_height=m_concent=0;

	m_number=0;
	m_left=-1;
	m_right=-1;

	m_fiducial=false;
	m_rider=FALSE;
	m_begingroup=FALSE;
}

cmpeek::cmpeek(double f, double b, int number, int left, int right)
{ 
	if (f < b) {
	    m_front = f; m_back = b;
	} else {
	    m_front = b; m_back = f;
	}

    m_front_height = m_back_height = 0;
	m_area=m_height=m_concent=0;

	m_number=number;
	m_left=left;
	m_right=right;

	m_fiducial=false;
	m_rider=FALSE;
	m_begingroup=FALSE;

}

BOOL cmpeek::IsFound(double t1, double t2)
{
    if (t1 >= m_front && t1 <= m_back ||	  // if exist intersection
		t2 >= m_front && t2 <= m_back ||	  // of (t1,t2)	and (front,back)
		m_front >= t1 && m_front <= t2 ||
		m_back >= t1 && m_back <= t2 ) {
	    return TRUE;
	} 
	return FALSE;
}

cmpeek::EDGES cmpeek::Edge(double t1, double t2)  {
	if( m_front >= t1 && m_front <= t2)  return FRONT;
	if( m_back >= t1 && m_back <= t2)  return BACK;
	return NOEDGE;
}

void cmpeek::SetBaseLine(double t1, double t2, const CLattice &data)
{
	double y1=data[t1], y2=data[t2];

	m_front_height = CLattice::approx(m_front, t1 ,t2, y1, y2); 
	m_back_height = CLattice::approx(m_back, t1 ,t2, y1, y2);
}

void cmpeek::SetProperties( const CLattice &data) 
{
	double dt=data.GetDt();
	int i1=int(ceil((m_front-data.GetT0())/dt)), //начало
		i2=int(floor((m_back-data.GetT0())/dt)); //и конец пика в номерах точек (внутри пика)

	if( i2>data.GetUpperBound())	i2=data.GetUpperBound();//если конец пика больше номера последнего элемента
	if( i1<0)	i1=0;

	double f0=data.GetTime(i1-1), f1=data.GetTime(i1);
	double b0=data.GetTime(i2), b1=data.GetTime(i2+1);

	double front_height = data[m_front],  back_height = data[m_back];//m_front & m_back задаются при создании пика

	if( m_left<0 && m_right<0 )  {	  // для неразделённых пиков зададим базовую линию
		m_front_height = front_height;	m_back_height = back_height;
	}

	if( i2<i1)	{  //если промежуток между началом пика и концом (реальные в-ны) меньше шага
		m_time   = (m_front+m_back)/2;
		m_height = (front_height+back_height)/2 - 
							(m_front_height+m_back_height)/2;
		m_area = m_height*dt;
		}  
	else  {
		m_time = 0;
		m_height = -DBL_MAX;
		m_area = 0;	
		for (int i = i1; i <= i2; i++) {

			double t = data.GetTime(i);
			double y = fabs(data[i] - CLattice::approx( t, 
						m_front, m_back, m_front_height, m_back_height));
			if (m_height < y) {
				m_time = t;		
				m_height = y;
				}
			m_area += data[i];
			}

		m_area *= dt;
		m_area -= dt*(data[i1]+data[i2])/2;
		m_area += (front_height+data[i1])*(data.GetTime(i1)-m_front)/2 
				+ (back_height+data[i2])*(m_back-data.GetTime(i2))/2;
	}

	// take in account the base line
	m_area -= (m_front_height + m_back_height) * (m_back-m_front)/ 2;

//	m_area=fabs(m_area); //remember!!! неправильно считаются вложеные пики
}


void cmpeek::Serialize(CArchive &ar, int ver, int number)
{
	if( ar.IsStoring())	 {
		ar<<m_front<<m_back<<m_concent<<m_comment<<m_right;
		//VERSION>=2 
		ar<<m_fiducial;

		TRACE("\nPeack %d",number);
		} 
	else {
		m_number = number;
		if( ver==0)	{
			PEEK_BUFF buff;
			if ( ar.Read(&buff, sizeof(buff)) != sizeof(buff))  {
				AfxThrowArchiveException( CArchiveException::badIndex, NULL);
			}
			m_front=buff.front;  m_back=buff.back;
			m_right=buff.right;  m_left=-1;
			m_concent=buff.concent;
			m_comment=buff.comment;
			m_fiducial=false;
		} else {
			ar>>m_front>>m_back>>m_concent>>m_comment>>m_right;
			if( ver>=2) ar>>m_fiducial;
		}
	}
	

	m_DopParams.Serialize( ar, ver );
}

const cmpeek& cmpeek::operator=( const cmpeek &peek1)  
{ 
	ASSERT( &peek1!=this);  

	m_time = peek1.m_time;
	m_area = peek1.m_area;
	m_height = peek1.m_height;
	m_back_height = peek1.m_back_height;
	m_front_height = peek1.m_front_height;
	m_front = peek1.m_front;
	m_back = peek1.m_back;
	m_concent = peek1.m_concent;
	m_number = peek1.m_number;
	m_left = peek1.m_left;
	m_right = peek1.m_right;
	m_comment = peek1.m_comment;
	m_fiducial = peek1.m_fiducial;

	m_rider		=peek1.m_rider;
	m_begingroup=peek1.m_begingroup;

	m_DopParams=peek1.m_DopParams;

	return *this;
}

	
//////////////////////////////////////////////////////////////////////
// CPeeksTable Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPeeksTable::CPeeksTable() : POINTS_PER_PEEK(15)
{
	m_fData=NULL;
//	m_t0=0;  m_dt=1;
	m_nPeekStart = m_nGroupStart = -1;
	m_peekThresh = m_groupThresh = 0;
	m_minHeight = m_minArea = m_minTime = m_minWidth = 0;
	m_Assim=1;
	m_FindType=FT_Begin;
//	m_bAtBack=false;
//	m_bAtFront=true;
	m_noise=0;
}

CPeeksTable::~CPeeksTable()
{

}

void CPeeksTable::Read(CFile &file, int stream_size /*used for old style*/)
{
	if( stream_size < 2*sizeof(int))  {
		clear();
		return;
	}

	ASSERT(m_fData!=NULL);

	int sign1, sign2, ver=0, num=0;
	file.Read( &sign1, sizeof(sign1));  file.Read( &sign2, sizeof(sign2));
	bool bNewStyle = (sign1==SIGNATURE1 && sign2==SIGNATURE2);

	if( !bNewStyle)  file.SeekToBegin();  // return to the begining

	CArchive ar( &file, CArchive::load);
	if( bNewStyle)   {
		ar>>ver;
		if( ver>VERSION)  AfxThrowArchiveException( CArchiveException::badIndex, NULL);
		ar>>num;
	} else num = stream_size/sizeof(cmpeek::PEEK_BUFF);

	resize(num);
	for ( unsigned int i = 0; i<size(); i++)  {
		at(i).Serialize( ar, ver, i);
	}
	
	m_CommonDopParams.Serialize(ar,ver);


	ar.Close();

    for( unsigned int i = 0; i < size(); i++) {
		int right=at(i).GetRight();
		if( right>=0 && right<size()) at(right).SetLeft(i);
		else at(i).SetRight(-1);
	}

	UpdatePeeks();
}

void CPeeksTable::Read(LPSTREAM pStream)
{
    USES_CONVERSION;
    ASSERT(pStream != NULL);

    ULARGE_INTEGER libNewPosition = {0, 0};
    LARGE_INTEGER startPosition = {0, 0};

    VERIFY(pStream->Seek(startPosition, STREAM_SEEK_END, &libNewPosition) == S_OK);
    VERIFY(pStream->Seek(startPosition, STREAM_SEEK_SET, NULL) == S_OK);

	COleStreamFile f(pStream);
	Read( f, libNewPosition.LowPart);
	f.Detach();
}

void CPeeksTable::Save(CFile &file)
{
	CArchive ar( &file, CArchive::store);

	int version=VERSION;

	// проверяем на наличие доп параметров, если их нет, то спускаемся к старой верии 2
	if( !IsDopParamExist() && !m_CommonDopParams.IsDopParamExist() )
		version=2; // старая версия файла

	ar<<SIGNATURE1<<SIGNATURE2<<version<<size();


	for (unsigned int i = 0; i<size(); i++)  {
		at(i).Serialize( ar, version);
	}

	m_CommonDopParams.Serialize(ar,version);
	
	ar.Close();
}

void CPeeksTable::Save(LPSTREAM pStream)
{
    ASSERT(pStream != NULL);

	COleStreamFile f(pStream);
	Save(f);
	f.Detach();
}

void CPeeksTable::UpdatePeeks()
{
	ASSERT( m_fData!=NULL);

	unsigned int i;

	// поиск цепочки пиков и установка одной базовой линии
	for ( i = 0; i < size(); i++)  {
		if( at(i).GetRight() < 0 || at(i).GetLeft()>=0) continue;  // not start of chain
		int last=at(i).GetRight(), j=last;
		while( (j=at(j).GetRight()) >= 0)  last=j;
		double t1=at(i).GetFront(), t2=at(last).GetBack();
		j=i;  do {
			at(j).SetBaseLine( t1, t2, *m_fData);
		} while( (j=at(j).GetRight()) >= 0);
	}

	// перевычисление площадей, высот, и времён выхода пиков
	for ( i = 0; i < size(); i++) {
		at(i).SetProperties( *m_fData);			
    }

	// вычитание площадей наездников из площади основного пика
	for ( i = 0; i < size(); i++) {		
		double front=at(i).GetFront(), back=at(i).GetBack();
		for (unsigned int j = 0; j < size(); j++) {
			if( i==j) continue;
			double front1=at(j).GetFront(), back1=at(j).GetBack();
			if( front1>=front && back1<=back)  at(i).SubsArea( at(j).GetArea());
		}
    }
}

void CPeeksTable::UpdateLinks()
{
	unsigned int i;
	for( i=0; i<size(); i++)  {
		int left=at(i).GetLeft(), right=at(i).GetRight();

		// stucked peek exists and was shifted, let's seek it
		bool badleft = (left>=0) && (left>=size() || !at(left).IsFound(left)),
			badright = (right>=0) && (right>=size() || !at(right).IsFound(right));
		if( badleft	|| badright)  {
			for (unsigned int j = 0; j<size(); j++)	{
				if(j==i) continue;
				if( badleft && at(j).IsFound(left))  {
					at(i).SetLeft(j);  badleft=false;	// left is valid now
					if( !badright) break;
				} else if( badright && at(j).IsFound(right))  {
					at(i).SetRight(j); 	badright=false;	  // right is valid now
					if( !badleft) break;
				}
			}
		}
	}

	for( i=0; i<size(); i++)  at(i).SetNumber(i);
}

                                       

bool CPeeksTable::AddPeek(double f, double b, bool Reorganize, int type )
{
	if( f>b)  { double temp=b; b=f; f=temp; }
//	if( FindPeek(f, b) >= 0) return false;

	cmpeek peek(f, b, size());
	peek.SetProperties( *m_fData);	// just to find	time
	peek.SetConcent(0);
	peek.m_rider = type;
	insert( end(), peek);
	if(Reorganize) 	{
		sort( begin(), end());
		UpdateLinks();
		UpdatePeeks();
	}

	return true;
}

bool CPeeksTable::AddStuckPeek(int basenum, double thisTime, cmpeek::EDGES edge)
{
	double frontTime;
	int i;
	
	if(basenum<0 && basenum>=size()) return false;

	int numnew=size();

	if(edge==cmpeek::AUTOMARK || edge==cmpeek::AUTOMARK_ENDGROUP ) {

		//поиск последнего пика в группе, но не наездника, либо просто начало группы
		int endGroup=-1;
		int beginGroup=-1;
		int endNum=back().GetNumber();
		double newFrontTime=0;
		double newFrontHeight=DBL_MAX;

		for(i=endNum; i>=0; i--) {
			cmpeek& deb=at(i);
			if(!at(i).IsRider()) { //найден не наездник
				endGroup=at(i).GetNumber();
				break;
				}

			if(at(i).IsBeginGroup()) {//найдено начало группы, но все предыдущие пики - наездники
				beginGroup=at(i).GetNumber();
				break;
				}

			//это пик-наездник в группе
			//найдём низжую точку по краям пиков-наездников, будем считать, что отсюда начинается неразделённый пик в группе
			if( at(i).GetBackHeight() < newFrontHeight ) {
				newFrontHeight=at(i).GetBackHeight();
				newFrontTime =at(i).GetBack();
				}
			if( at(i).GetFrontHeight() < newFrontHeight ) {
				newFrontHeight=at(i).GetFrontHeight();
				newFrontTime =at(i).GetFront();
				}
			}

		if(endGroup<0&&beginGroup<0)  //не найдено ни начала группы, ни последнего обычного пика (не наездника)
			return false;

		if(endGroup>=0) { //в группе найден последний обычный пик (не наездник)
			if(newFrontTime>0) { //перед этим, найденным пиком были найдены наездники
				frontTime=newFrontTime;
				at(endGroup).SetBack(frontTime);//переместим конец последнего найденного обычного пика в группкеанного пика к краю предыдущего наездника, где сигнал минимален
				}
			else //если пиков-наездников между последним пиком в группе и добавленным пиком небыло
				frontTime=at(endGroup).GetBack(); 


			cmpeek peek( frontTime, thisTime, numnew, endGroup);
			insert( end(), peek);
			at(endGroup).SetRight(numnew);
			return true;
			}

		if(beginGroup>=0) { //группа начинается с пика-наездника
			frontTime=at(beginGroup).GetFront();

			cmpeek peek( frontTime, thisTime, numnew ); //добавляем первый обычный пик (от начала группы, от самого первого пика-наездника)
			insert( end(), peek);
			return true;
			}
		}

	if(edge==cmpeek::BACK) {
		ASSERT( at(basenum).GetRight() < 0);
		double front=at(basenum).GetBack();
		if( front>=thisTime) return false;
		cmpeek peek( front, thisTime, numnew, basenum);
		insert( end(), peek);
		at(basenum).SetRight(numnew);
		}

	if(edge==cmpeek::FRONT) {
		ASSERT( at(basenum).GetLeft() < 0);
		double back=at(basenum).GetFront();
		if( back<=thisTime) return false;
		cmpeek peek( thisTime, back, numnew, -1, basenum);
		insert( end(), peek);
		at(basenum).SetLeft(numnew);
		}

	if(edge!=cmpeek::AUTOMARK)  {
		UpdatePeeks();
		sort( begin(), end());
		UpdateLinks();
	}
 
	return true;
}

bool CPeeksTable::ModifyPeek(int num, double f, double b)  {

	if( f>b)  return false; //{ double temp=b; b=f; f=temp; }

	cmpeek& peek = at(num);
	int left=peek.GetLeft(), right=peek.GetRight();

	if( left>=0 || right>=0)	{
		if( left>=0) if( f<at(left).GetFront()) return false;
		if( right>=0) if( b>at(right).GetBack()) return false;

		peek.SetFront(f); peek.SetBack(b);
		if( left>=0) at(left).SetBack(f);
		if( right>=0)  at(right).SetFront(b);

	} else {
		peek.SetFront(f); peek.SetBack(b);
		peek.SetProperties(*m_fData);
	}

	UpdatePeeks();
    return true;
}

void CPeeksTable::Reorganize()
{
	UpdatePeeks();
	sort( begin(), end());
	UpdateLinks();
}


void CPeeksTable::MakeGroups()
{
BOOL  find_group=FALSE;
//double start_group=0;
//double stop_group=0;
int    start_number=0;
int    stop_number=0;

for (unsigned int i = 0; i<size(); i++)  {
	cmpeek &peek = at(i);
/*	if( peek.IsRider() ) {
		TRACE("\nПик наездник %d",i+1);
		continue;
		}
*/
	if(	!peek.IsGroupFlag() ) { // уже не в групе - закончим группу

		if(!find_group) { // если небыло группы
//			start_group=peek.GetFront();
			start_number=i;
			TRACE("\nПик без группы %d",i+1);
			continue;
			}
		else { // была группа а теперь нет 
		// основное 
			TRACE("\n  -= Первичная группа =-");
			//CheckAndMarkGroup( start_number, stop_number );
			find_group=FALSE;
			start_number=i;
			}
		}
	else { // бродим по группе
//			stop_group=peek.GetBack();
			stop_number=i;
			find_group=TRUE;
		}
	}

if(find_group) {// последний пик был в группе - эту группу тоже добавим
	TRACE("\n  -= Первичная группа =-");
	//CheckAndMarkGroup( start_number, stop_number );
	}

}




BOOL CPeeksTable::IsPossibleAutomartOnEvent(double time1,double time2) {


	int ne=0;
	CompEvent CETemp;
	double time_start, time_stop=-1;
	
	while (EList.Lookup(ne,CETemp))	{

		if(CETemp.EventNum==0) // запретить разметку
			if( time_stop == -1 ) // первое вхождение запрещения (до следующего разрешения)
				time_stop=CETemp.EventStart;
		
		if(CETemp.EventNum==1 && time_stop != -1) {// разрешить разметку
			time_start=CETemp.EventStart;
			if( (time1>time_stop && time1<time_start) || (time2>time_stop && time2<time_start) )
				return FALSE;
			time_stop=-1;
			}
		ne++;
		}

if(time_stop != -1 ) // было лишь одно запрещение
	if(time1>time_stop || time2>time_stop) 
		return FALSE;

return TRUE;
}





float CPeeksTable::CalcSimplePeekArea(int i_left, int i_right )
{
/*float dt=m_fData->GetDt();
float t0=m_fData->GetT0();

	cmpeek peek1( i_left*dt+t0, i_right*dt+t0, 0);
	peek1.SetProperties(m_buf);

	return peek1.GetArea();

	
*/
float summ_A=0;
const float* data=m_fData->GetData();

if( i_left<0 ) i_left=0;
if( i_right>=m_fData->GetSize() ) i_right=m_fData->GetSize()-1;


for( int i=i_left; i<=i_right; i++)
	summ_A+=data[i];
summ_A-=(data[i_left]+data[i_right])/2;

return (summ_A-0.5*(i_right-i_left)*(data[i_left]+data[i_right]))*m_fData->GetDt();	

  
}

float CPeeksTable::CalcSimplePeekAreaBuff(int i_left, int i_right, const float* buff, int N, float dt )
{
float summ_A=0;

if( i_left<0 ) i_left=0;
if( i_right>=N ) i_right=N-1;


for( int i=i_left; i<=i_right; i++)
	summ_A+=buff[i];
summ_A-=(buff[i_left]+buff[i_right])/2;

return (summ_A-0.5*(i_right-i_left)*(buff[i_left]+buff[i_right]))*dt;	
}

//====================================================================//
// Проверяет и, если пик большой, добавляет пик  
//   - отдельный, если группа не начата (ориентируясь на конец данного 
//              отдельного пика начинает или нет группу)
//   - неразделённый с последним (ориентируясь на конец данного 
//              неразделённого пика завершает или нет группу)
//====================================================================//
void CPeeksTable::CheckAndAddPeek(int i)
{

if( m_nPeekStart<0 || i<0 ) return;

//double y=m_buf[i];
//double width; // ширина группы

cmpeek peek( m_fData->GetTime(m_nPeekStart), m_fData->GetTime(i), 0);
peek.SetProperties(*m_fData); //задает площадь, вершину и значение в вершине

//CorrectBaseLine(peek,FALSE);

peek.CheckRider(m_riderHeight);

double corr_start=peek.GetFront();
double corr_end  =peek.GetBack();

if( 1) {// peek.GetArea() > m_minArea && peek.GetHeight() > m_minHeight ) { 
	//если пик достаточно большой


	if( fabs(peek.GetTime()-back().GetTime())< m_groupThresh*m_peekThresh*(1+i/m_Assim*m_fData->GetDt())) {   
		// если расстояние до предыдущего пика мало 
	
		//проверка на наездника
		if( peek.IsRider() ) { //если это наездник; то есть если высота этого (отдельностоящего) пика меньше максимальной высоты наездников
//			ExpandLastGroupPeek(m_buf.GetTime(i)); //расширим последний пик группы до текущего момента
			AddPeek( corr_start, corr_end, false, cmpeek::PT_Rider);
			}
		else { //если это не наездник, а обычный неразделённый пик		
				
			// добавим неразделённый с последним пиком
			AddStuckPeek( size()-1, m_fData->GetTime(i), cmpeek::AUTOMARK); 
			}
		}
	else { // если расстояние велико
//		m_nGroupStart=-1;					   // закончим группу
		AddPeek( corr_start, corr_end, false); // добавляем скорректированый пик
		} 

		// добавляем пик без корекции от начала пика до текущего момента, сортировать пики не надо
		//	AddPeek( m_buf.GetTime(m_nPeekStart), m_buf.GetTime(i), false); 
			
//		m_nGroupStart=m_nPeekStart; //начнём группу
//		back().SetBeginGroup();
//	}
//		else  //если это отдельный пик
//		} 
 
	}

m_nPeekStart=i;
}

void CPeeksTable::MarkRiders()
{

int left=-1, right=-1;
/*
for( int i=0; i<size(); i++)  {
		
	const cmpeek &peek = at(i);
		
	left=peek.GetLeft();
	rigth=peek.GetRight();

	if( left<0 && right<0) continue; // если это отдельный пик (не в группе)
		
	cmpeek peek1( peek.GetFront(), peek.GetBack(), 0);   // такой же пик, но если бы он был отдельно стоящим
	peek1.SetProperties( m_buf);

	double minHeight  =GetMinHeight(peek.GetFront());
	double minArea	  =GetMinArea(peek.GetFront());
	double riderHeight =GetRiderHeight(peek.GetFront());
		
	if( peek1.GetHeight() < riderHeight ) { //если это наездник; то есть если высота этого (отдельностоящего) пика меньше порога наездников
	
		if( peek1.GetArea()<minArea || peek1.GetHeight() < minHeight ) //очень маленький пик
			DeletePeek(peek.GetNumber(), false);

		if( left >=0 ) {		//если у этого пика есть неразделённый слева
			if( right >=0 )	{	//а также неразделённый справа
			
				at(left).SetRight(right);	//у левого пика скажем, что справа у него неразделённый уже другой
				at(right).SetLeft(left);	//а у того другого слева тоже поменялся сосед
				peek.SetLeft(-1);			//а текущий пик сам по себе
				peek.SetRight(-1);
				}
			else {							//если это был последний пик в цепочке
				at(left).SetRight(right);	//левому пику скажем, что он последний
				peek.SetLeft(-1);			//а текущий пик сам по себе
				}
			}
		else {					//если данный пик первый в цепочке; у этого пика нет неразделённых слева
								//справа неразделённый точно есть
			at(right).SetLeft(-1);	//неразделённый справа становится первым в цепочке
			peek.SetRight(-1);		//а текущий пик сам по себе
			}
		}// конец обработке данного наездника
	}//for

  */
}

const CPeeksTable& CPeeksTable::operator=( const CPeeksTable &table1)
{
	clear();
	resize( table1.size());
	iterator it;
	const_iterator it1;
	for( it1=table1.begin(), it=begin(); it1!=table1.end(); it1++, it++)  {
		*it = *it1;
	}

	return *this;
}



BOOL CPeeksTable::IsDopParamExist()
{
	for (unsigned int i = 0; i<size(); i++) {
		if( at(i).IsDopParamExist() ) return TRUE;
	}
	return FALSE;
}

int CPeeksTable::FindPeek( LPCTSTR str)
{
	for (unsigned int i = 0; i<size(); i++) {
		const CString &name = at(i).GetComment();
		if( !name.IsEmpty() && name.CollateNoCase(str) == 0)
			return i;
	}
	return -1;
}

void CPeeksTable::CorrectTime(double k_corr, double t0)
{
	for( iterator it=begin(); it!=end(); it++)  {
		it->SetFront( (it->GetFront() - t0)*k_corr + t0);
		it->SetBack( (it->GetBack() - t0)*k_corr + t0);
	}
	UpdatePeeks();
}

int CPeeksTable::Found(int posp)
{
	double posd=posp*m_fData->GetDt()+m_fData->GetT0();
	for (unsigned int i = 0; i<size(); i++)
	{
		const cmpeek &peek = at(i);
		if ( (peek.GetFront()<posd) && (peek.GetBack()>posd) ) return i;
	}
	return -1;
}

double CPeeksTable::GetMaxHeight()
{
	double CurMax=-DBL_MAX;
	for (int i=0; i<size(); i++)
	{
		const cmpeek &peek = at(i);
		if ( peek.GetHeight()>CurMax ) CurMax=peek.GetHeight();
	}
	return CurMax;
}


void CPeeksTable::KorrectMark()
{
	for (unsigned int i = 0; i<size(); i++)  {
		const cmpeek &peek = at(i);

	}

}

void cmpeek::CheckRider(double hRider)
{
if(m_area<hRider) m_rider|=PT_Rider;
else		 	  m_rider&=(!PT_Rider);
}

void CPeeksTable::CorrectBaseLine(cmpeek &peek, BOOL isGroup)
{
	double dt=m_fData->GetDt();
	double t0=m_fData->GetT0();

	int ibegin	=int(ceil ((peek.GetFront()-t0)/dt)), //начало
		iend	=int(floor((peek.GetBack() -t0)/dt)), //и конец пика в номерах точек (внутри пика)
		iversh	=int(	   (peek.GetTime() -t0)/dt ); //вершина

	if( iversh<ibegin ) iversh=ibegin;
	if( iversh>iend )   iversh=iend;

		
	if( ibegin<0)	ibegin=0;
	if( iend>m_fData->GetUpperBound())	iend=m_fData->GetUpperBound();//если конец пика больше номера последнего элемента

	if(peek.GetFrontHeight()<peek.GetBackHeight() ) { //начало пика снизу

		int ibest=ibegin;
		int iendcounter=ibegin+(iversh-ibegin)*0.7; //просматриваем лишь часть пика (а иначе микро пики на конце сделают этот пик совсем маленьким)
		for( int i=ibegin; i<iendcounter; i++) {
			if( i==iend ) continue;
			double y1=m_fData->GetAt(i);
			double y2=m_fData->GetAt(iend);
			double ynext=m_fData->GetAt(i+1);
			double y=y1+(y2-y1)/(iend-i);
			double delta=y-ynext;
			if(delta>0) 
				ibest=i;
			}
		ibest++;
		if(ibest>=iend) return;

		peek.SetFront( m_fData->GetTime(ibest));
		peek.SetProperties(*m_fData);
		}

	if(peek.GetFrontHeight()>=peek.GetBackHeight() ) {

		int ibest=iend;
		int iendcounter=iversh+(iend-iversh)*0.3; //просматриваем лишь часть пика (а иначе микро пики на конце сделают этот пик совсем маленьким)
		for( int i=iend; i>iendcounter; i--) {
			if( i == ibegin ) continue;
			double y1=m_fData->GetAt(ibegin);
			double y2=m_fData->GetAt(i);
			double ynext=m_fData->GetAt(i-1);
			double y=y1+(y2-y1)/(i-ibegin)*(i-1-ibegin);
			double delta=y-ynext;
			if(delta>0) 
				ibest=i;
			}
		ibest--;
		if(ibest<=ibegin) return;

		peek.SetBack( m_fData->GetTime(ibest));
		peek.SetProperties(*m_fData);
	}
}

void CPeeksTable::ExpandLastGroupPeek(double thisTime)
{

	//поиск последнего пика в группе, но не наездника
	int endNum=back().GetNumber();

	for(int i=endNum; i>=0; i--) {
		if(!at(i).IsRider()) { //найден не наездник
			at(i).SetBack(thisTime); //расширим данный пик до текущего времени
			break;
			}
		}
}

void cmpeek::DopParamList::SetParam(CString &name, CString &value)
{

	if( name.IsEmpty() ) //funnily
		return;

	vector<TwinsDopParam>::iterator it;

	for( it=begin(); it!=end(); it++) {

		if(it->name==name) {
			it->value=value; 
			return;
			}

		}
	
	push_back(TwinsDopParam(name,value));

}


CString cmpeek::DopParamList::GetParam(CString &name)
{
vector<TwinsDopParam>::iterator it;

for( it=begin(); it!=end(); it++) 
	if(it->name==name) 
		return it->value; 
	
return CString();

}

BOOL cmpeek::DopParamList::IsParamExist(CString &name)
{
vector<TwinsDopParam>::iterator it;

for( it=begin(); it!=end(); it++) 
	if(it->name==name) 
		return TRUE; 
	
return FALSE;
}

void CPeeksTable::LoadDopParamNames(cmpeek::DopParamList* list)
{
list->clear();

for( iterator it=begin(); it!=end(); it++)  { // по пикам
	vector<cmpeek::TwinsDopParam>::iterator itc;
	for( itc=it->m_DopParams.begin(); itc!=it->m_DopParams.end(); itc++)  // по доп параметрам
		list->SetParam(itc->name,itc->value);
	}
}


const cmpeek::DopParamList&  cmpeek::DopParamList::operator=( const cmpeek::DopParamList& list1 )  {

	clear();
	if( list1.size()==0 ) {
		return (*this);
		}
	
	vector<TwinsDopParam>::const_iterator it;

	TRACE("\nCopy Dop Param List (%d): ",list1.size());
	for( it=list1.begin(); it!=list1.end(); it++) {
		TRACE(" {%s-%s}", it->name, it->value);
		push_back( (*it) );
		}

	return (*this);

}


void cmpeek::DopParamList::Serialize(CArchive &ar, int ver)
{
	if( ver<3 ) return;

	vector<TwinsDopParam>::iterator it;

	TRACE(" , dopparam (%d):",size());

	if( ar.IsStoring())	 {
		ar<<unsigned int(size());
		for( it=begin(); it!=end(); it++) {
			ar<<it->name; 
			ar<<it->value; 

			TRACE(" [%s,%s]",it->name,it->value);
			}
		} 
	else {
		unsigned int sz;
		ar>>sz;
		if( sz==0 ) clear();
		else {

			if( ver==3 ) {  // сразу проверяем на совпадения имён и пустые имена
				for (unsigned int i = 0; i<sz; i++) {

					TwinsDopParam tdp;
					ar>>tdp.name; 
					ar>>tdp.value;

					if( tdp.name.IsEmpty() ) continue;
					if( IsParamExist(tdp.name) ) continue;
					
					push_back(tdp); 
					}
				}
			else {
				resize(sz);	
				for( it=begin(); it!=end(); it++) {
					ar>>it->name; 
					ar>>it->value;
					}
				}
			}
		}
}





CString CPeeksTable::GetPeekTableStr()
{

CString out;
/*Не уверен*/
/*for( iterator it=begin(); it!=end(); it++)  {
	out+=it.GetComponentDataStr();
	out+="\r\n";
	}
*/
return out;
}


/*CString cmpeek::GetComponentDataStr()
{
CString sdata,one;

one.Format("%.3f\t",m_time);
sdata+=one;

MakePrecisionString(one, m_height,1);
sdata+=one;
sdata+="\t";

MakePrecisionString(one, m_area,2);
sdata+=one;
sdata+="\t";

MakePrecisionString(one, m_concent,0);
sdata+=one;
sdata+="\t";

sdata+="\"";
sdata+=m_comment;
sdata+="\"";

return sdata;

}*/
