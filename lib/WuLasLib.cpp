// WuLasLib.cpp : Defines the initialization routines for the DLL.
//
#include "WuLasLib.h"

#include <float.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <iostream>

#include "laswriter_las.hpp"
#include "lasreader_las.hpp"

// lastool不是一次读取,所以没法用
const int MEMORY_SIZE = 100 * 1024 * 1024;

CWuLasLib::CWuLasLib()
{
	// 初始化
	memset(m_szErrorMsg, 0, sizeof(char) * 512);
	memset(m_szLasPath,  0, sizeof(char) * 512);

	m_xOffset = 0;
	m_yOffset = 0;
	m_zOffset = 0;

	m_xScalefactor = 1.0;
	m_yScalefactor = 1.0;
	m_zScalefactor = 1.0;

	// 文件标志
	//m_pfpFile   = NULL;
	m_pOpener = NULL;
	m_pLasMode  = NULL;
	m_bOpenMode = false;

	// 初始化范围
	m_xMin = DBL_MAX;
	m_xMax = DBL_MIN;

	m_yMin = DBL_MAX;
	m_yMax = DBL_MIN;

	m_zMin = DBL_MAX;
	m_zMax = DBL_MIN;

	m_nPoint = 0;
}

CWuLasLib::~CWuLasLib()
{
	Reset();
}

void CWuLasLib::Reset()
{
	// 初始化
	memset(m_szErrorMsg, 0, sizeof(char) * 512);
	memset(m_szLasPath,  0, sizeof(char) * 512);
	
	m_xOffset = 0;
	m_yOffset = 0;
	m_zOffset = 0;
	
	m_xScalefactor = 1.0;
	m_yScalefactor = 1.0;
	m_zScalefactor = 1.0;
	
	if (m_bOpenMode){
		Close();
	}
	else{
		//m_pfpFile  = NULL;
		m_pOpener = NULL;
		m_pLasMode = NULL;
	}
	
	// 初始化范围
	m_xMin = DBL_MAX;
	m_xMax = DBL_MIN;
	
	m_yMin = DBL_MAX;
	m_yMax = DBL_MIN;
	
	m_zMin = DBL_MAX;
	m_zMax = DBL_MIN;

	m_nPoint = 0;
}

const char * CWuLasLib::GetLastError()
{
	return m_szErrorMsg;
}

/*
bool CWuLasLib::Open( const char * pszLasFile, OPENFLAGS flag)
{
	Reset();

	strcpy(m_szLasPath, pszLasFile);
	m_flag = flag;

	// 文件操作
	if (m_flag == modeRead){
		m_pfpFile = fopen(pszLasFile, "rb");
		if (m_pfpFile == NULL){
			sprintf(m_szErrorMsg, "%s can not open!\n", pszLasFile);
			return false;
		}

		m_pLasMode = new LASreaderLAS();
		bool bOpenSuccess = ((LASreaderLAS * )m_pLasMode)->open(m_pfpFile);
		
		if (bOpenSuccess == false){
			strcpy(m_szErrorMsg, "Can not read las file header!");
			delete m_pLasMode;				m_pLasMode = NULL;
			fclose(m_pfpFile);				m_pfpFile  = NULL;
			return false;
		}

		ReadLasHeader();
	}
	else{
		m_pfpFile = fopen(pszLasFile, "wb");
		if (m_pfpFile == NULL){
			sprintf(m_szErrorMsg, "%s can not open!\n", pszLasFile);
			return false;
		}
		// check the extention of the file

		const char * pszdot = strrchr(pszLasFile, '.') + 1;
		if (strcmp(pszdot + 1, "laz") == 0 ||strcmp(pszdot + 1, "LAZ") == 0){
			m_bCompress = true;
		}
		else{
			m_bCompress = false;
		}
	}
	m_bOpenMode = true;

	return true;
}
*/

bool CWuLasLib::Open( const char * pszLasFile, OPENFLAGS flag)
{
	Reset();

	strcpy(m_szLasPath, pszLasFile);
	m_flag = flag;

	if (m_flag == modeRead){
		m_pOpener = (LASreadOpener *)new LASreadOpener();
		if (m_pOpener == NULL){
			std::cout << "Can not create read pointer." << std::endl;
			return false;
		}
		((LASreadOpener *)m_pOpener)->set_file_name(pszLasFile);
		m_pLasMode = (LASreaderLAS * )(((LASreadOpener *)m_pOpener)->open());
		if (m_pLasMode == NULL){
			strcpy(m_szErrorMsg, "Can not read las file header!");
			delete (LASreadOpener *)m_pOpener;				m_pOpener = NULL;
			return false;
		}
		ReadLasHeader();
	}
	else{
		m_pOpener = (LASwriteOpener *)new LASwriteOpener();
		if (m_pOpener == NULL){
			std::cout << "Can not create read pointer." << std::endl;
			return false;
		}
		((LASwriteOpener *)m_pOpener)->set_file_name(pszLasFile);

		const char * pszdot = strrchr(pszLasFile, '.') + 1;
		if (strcmp(pszdot + 1, "laz") == 0 ||strcmp(pszdot + 1, "LAZ") == 0){
			m_bCompress = true;
		}
		else{
			m_bCompress = false;
		}
	}

	m_bOpenMode = true;

	return true;
}

void CWuLasLib::SetOffset( double xOffset, double yOffset, double zOffset )
{
	m_xOffset = xOffset;
	m_yOffset = yOffset;
	m_zOffset = zOffset;
}

void CWuLasLib::SetPrecision( double xPrecision, double yPrecision, double zPrecision )
{
	m_xScalefactor = xPrecision;
	m_yScalefactor = yPrecision;
	m_zScalefactor = zPrecision;
}

// 只在写的情况设置头文件
bool CWuLasLib::WriteLasHeader( bool bColor /*= true*/ )
{
	m_pHeader = new LASheader();
	LASheader * pHeader = (LASheader *)m_pHeader;
	
	pHeader->version_major = 1;
	pHeader->version_minor = 2;
	if (bColor){
		pHeader->point_data_format = 2;
		pHeader->point_data_record_length = 26;
	}
	else{
		pHeader->point_data_format = 0;
		pHeader->point_data_record_length = 20;
	}
	
	strcpy(pHeader->system_identifier, "LasTool");
	strcpy(pHeader->generating_software, "WuLasLib");

	time_t long_time;
	time( &long_time );                
	struct tm * pTime = localtime( &long_time ); 

	pHeader->file_creation_year = pTime->tm_year + 1900;
	pHeader->file_creation_day  = pTime->tm_yday;

	pHeader->x_scale_factor = m_xScalefactor;
	pHeader->y_scale_factor = m_yScalefactor;
	pHeader->z_scale_factor = m_zScalefactor;

	pHeader->x_offset = m_xOffset;
	pHeader->y_offset = m_yOffset;
	pHeader->z_offset = m_zOffset;
/*
	m_pLasMode = new LASwriterLAS();
	bool bOpenSuccess = false;
	if (m_bCompress){
		bOpenSuccess = ((LASwriterLAS * )m_pLasMode)->open(m_pfpFile, pHeader, LASZIP_COMPRESSOR_LAYERED_CHUNKED);
	}
	else{
		bOpenSuccess = ((LASwriterLAS * )m_pLasMode)->open(m_pfpFile, pHeader, 0);
	}

	if (bOpenSuccess == false){
		strcpy(m_szErrorMsg, "Can not write las file header!");
		delete m_pLasMode;				m_pLasMode = NULL;
	}
*/
	m_pLasMode = (LASwriter * )(((LASwriteOpener *)m_pOpener)->open(pHeader));

	if (m_pLasMode == NULL){
		strcpy(m_szErrorMsg, "Can not read las file header!");
		delete (LASreadOpener *)m_pOpener;				m_pOpener = NULL;
		return false;
	}
	return true;
}

bool CWuLasLib::WriteLas( LasPoint * pLasPoint, int nPoint )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}

	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}

	if (m_pLasMode == NULL){
		if (! WriteLasHeader() ){
			return false;
		}
	}

	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;

	// 写点云文件
	int i;
	
	LASpoint pt;
	LASheader * pHeader = (LASheader *)m_pHeader;
	pt.init(pHeader, pHeader->point_data_format, pHeader->point_data_record_length, 0);
	for (i = 0; i < nPoint; ++i, ++pLasPoint){
		if (m_xMin > pLasPoint->x)	m_xMin = pLasPoint->x;
		if (m_xMax < pLasPoint->x)	m_xMax = pLasPoint->x;
		if (m_yMin > pLasPoint->y)	m_yMin = pLasPoint->y;
		if (m_yMax < pLasPoint->y)	m_yMax = pLasPoint->y;
		if (m_zMin > pLasPoint->z)	m_zMin = pLasPoint->z;
		if (m_zMax < pLasPoint->z)	m_zMax = pLasPoint->z;
		Global2LocalCoord(pLasPoint->x, pLasPoint->y, pLasPoint->z, pt.X, pt.Y, pt.Z);
		//pLasWriter->write_point(& pt, 0, pLasPoint->rgb);
		for (int j = 0; j < 3; ++j){
			pt.rgb[j] = pLasPoint->rgb[j];
		}
		pLasWriter->write_point(& pt);
		++m_nPoint;
	}

	return true;
}

bool CWuLasLib::WriteLas( LasPointClass * pLasPoint, int nPoint )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}
	
	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}
	
	if (m_pLasMode == NULL){
		if (! WriteLasHeader() ){
			return false;
		}
	}
	
	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;
	
	// 写点云文件
	int i;
	LASpoint pt;
	LASheader * pHeader = (LASheader *)m_pHeader;
	pt.init(pHeader, pHeader->point_data_format, pHeader->point_data_record_length, 0);
	for (i = 0; i < nPoint; ++i, ++pLasPoint){
		if (m_xMin > pLasPoint->x)	m_xMin = pLasPoint->x;
		if (m_xMax < pLasPoint->x)	m_xMax = pLasPoint->x;
		if (m_yMin > pLasPoint->y)	m_yMin = pLasPoint->y;
		if (m_yMax < pLasPoint->y)	m_yMax = pLasPoint->y;
		if (m_zMin > pLasPoint->z)	m_zMin = pLasPoint->z;
		if (m_zMax < pLasPoint->z)	m_zMax = pLasPoint->z;
		Global2LocalCoord(pLasPoint->x, pLasPoint->y, pLasPoint->z, pt.X, pt.Y, pt.Z);
		pt.intensity      = pLasPoint->intensity;
		pt.classification = pLasPoint->classification;
		//pLasWriter->write_point(& pt, 0, pLasPoint->rgb);
		for (int j = 0; j < 3; ++j){
			pt.rgb[j] = pLasPoint->rgb[j];
		}
		pLasWriter->write_point(& pt);
		++m_nPoint;
	}
	
	return true;
}

bool CWuLasLib::WriteLas( LasPointFull * pLasPoint, int nPoint )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}
	
	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}
	
	if (m_pLasMode == NULL){
		if (! WriteLasHeader() ){
			return false;
		}
	}
	
	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;
	
	// 写点云文件
	int i;
	LASpoint pt;
	LASheader * pHeader = (LASheader *)m_pHeader;
	pt.init(pHeader, pHeader->point_data_format, pHeader->point_data_record_length, 0);
	for (i = 0; i < nPoint; ++i, ++pLasPoint){
		if (m_xMin > pLasPoint->x)	m_xMin = pLasPoint->x;
		if (m_xMax < pLasPoint->x)	m_xMax = pLasPoint->x;
		if (m_yMin > pLasPoint->y)	m_yMin = pLasPoint->y;
		if (m_yMax < pLasPoint->y)	m_yMax = pLasPoint->y;
		if (m_zMin > pLasPoint->z)	m_zMin = pLasPoint->z;
		if (m_zMax < pLasPoint->z)	m_zMax = pLasPoint->z;
		Global2LocalCoord(pLasPoint->x, pLasPoint->y, pLasPoint->z, pt.X, pt.Y, pt.Z);
		pt.intensity       = pLasPoint->intensity;
		pt.classification  = pLasPoint->classification;
		pt.point_source_ID = pLasPoint->point_source_ID;
		//pLasWriter->write_point(& pt, 0, pLasPoint->rgb);
		for (int j = 0; j < 3; ++j){
			pt.rgb[j] = pLasPoint->rgb[j];
		}
		++m_nPoint;
	}
	
	return true;
}

bool CWuLasLib::WriteLas( LasPointEcho * pLasPoint, int nPoint )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}

	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}

	if (m_pLasMode == NULL){
		if (! WriteLasHeader() ){
			return false;
		}
	}

	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;

	// 写点云文件
	int i;
	LASpoint pt;
	LASheader * pHeader = (LASheader *)m_pHeader;
	pt.init(pHeader, pHeader->point_data_format, pHeader->point_data_record_length, 0);
	for (i = 0; i < nPoint; ++i, ++pLasPoint){
		if (m_xMin > pLasPoint->x)	m_xMin = pLasPoint->x;
		if (m_xMax < pLasPoint->x)	m_xMax = pLasPoint->x;
		if (m_yMin > pLasPoint->y)	m_yMin = pLasPoint->y;
		if (m_yMax < pLasPoint->y)	m_yMax = pLasPoint->y;
		if (m_zMin > pLasPoint->z)	m_zMin = pLasPoint->z;
		if (m_zMax < pLasPoint->z)	m_zMax = pLasPoint->z;
		Global2LocalCoord(pLasPoint->x, pLasPoint->y, pLasPoint->z, pt.X, pt.Y, pt.Z);
		pt.intensity       = pLasPoint->intensity;
		pt.classification  = pLasPoint->classification;
		pt.return_number   = pLasPoint->echo;
		pt.number_of_returns = pLasPoint->sumechos;
		//pLasWriter->write_point(& pt, 0, pLasPoint->rgb);
		for (int j = 0; j < 3; ++j){
			pt.rgb[j] = pLasPoint->rgb[j];
		}
		pLasWriter->write_point(& pt);
		++m_nPoint;
	}

	return true;
}

bool CWuLasLib::WriteLas( DPT3D * pLasPoint, int nPoint )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}
	
	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}
	
	if (m_pLasMode == NULL){
		if (! WriteLasHeader(false) ){
			return false;
		}
	}
	
	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;
	
	// 写点云文件
	int i;
	LASpoint pt;
	LASheader * pHeader = (LASheader *)m_pHeader;
	pt.init(pHeader, pHeader->point_data_format, pHeader->point_data_record_length, 0);
	for (i = 0; i < nPoint; ++i, ++pLasPoint){
		if (m_xMin > pLasPoint->x)	m_xMin = pLasPoint->x;
		if (m_xMax < pLasPoint->x)	m_xMax = pLasPoint->x;
		if (m_yMin > pLasPoint->y)	m_yMin = pLasPoint->y;
		if (m_yMax < pLasPoint->y)	m_yMax = pLasPoint->y;
		if (m_zMin > pLasPoint->z)	m_zMin = pLasPoint->z;
		if (m_zMax < pLasPoint->z)	m_zMax = pLasPoint->z;
		Global2LocalCoord(pLasPoint->x, pLasPoint->y, pLasPoint->z, pt.X, pt.Y, pt.Z);
		pLasWriter->write_point(& pt);
		++m_nPoint;
	}
	
	//std::cout << "xmin xmax : " << m_xMin << "\t" << m_xMax << std::endl;
	//std::cout << "ymin ymax : " << m_yMin << "\t" << m_yMax << std::endl;
	//std::cout << "zmin zmax : " << m_zMin << "\t" << m_zMax << std::endl;
	
	return true;
}

bool CWuLasLib::WriteLas( int nStartIndex, LasPoint * pLasPoint, int nListSize )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}
	
	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}
	
	if (m_pLasMode == NULL){
		if (! WriteLasHeader() ){
			return false;
		}
	}
	
	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;

	pLasWriter->Offset(nStartIndex);

	WriteLas(pLasPoint, nListSize);

	return true;
}

bool CWuLasLib::WriteLas( int nStartIndex, LasPointClass * pLasPoint, int nListSize )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}
	
	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}
	
	if (m_pLasMode == NULL){
		if (! WriteLasHeader() ){
			return false;
		}
	}
	
	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;
	
	pLasWriter->Offset(nStartIndex);
	
	WriteLas(pLasPoint, nListSize);
	
	return true;
}

bool CWuLasLib::WriteLas( int nStartIndex, LasPointFull * pLasPoint, int nListSize )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}
	
	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}
	
	if (m_pLasMode == NULL){
		if (! WriteLasHeader() ){
			return false;
		}
	}
	
	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;
	
	pLasWriter->Offset(nStartIndex);
	
	WriteLas(pLasPoint, nListSize);
	
	return true;
}

bool CWuLasLib::WriteLas( int nStartIndex, LasPointEcho * pLasPoint, int nListSize )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}

	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}

	if (m_pLasMode == NULL){
		if (! WriteLasHeader() ){
			return false;
		}
	}

	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;

	pLasWriter->Offset(nStartIndex);

	WriteLas(pLasPoint, nListSize);

	return true;
}

bool CWuLasLib::WriteLas( int nStartIndex, DPT3D * pLasPoint, int nListSize )
{
	if (! m_bOpenMode){
		strcpy(m_szErrorMsg, "File open fail!\n");
		return false;
	}
	
	if (m_flag == modeRead){
		strcpy(m_szErrorMsg, "File is only read!\n");
		return false;
	}
	
	if (m_pLasMode == NULL){
		if (! WriteLasHeader(false) ){
			return false;
		}
	}
	
	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;
	
	pLasWriter->Offset(nStartIndex);
	
	WriteLas(pLasPoint, nListSize);

	return true;
}

void CWuLasLib::Global2LocalCoord( double gx, double gy, double gz, int & x, int & y, int & z )
{
	double local;
	
	local = (gx - m_xOffset)/m_xScalefactor;
	if (gx < m_xOffset){
		x = (int)floor(local);
	}
	else{
		x = (int)ceil(local);
	}

	local = (gy - m_yOffset)/m_yScalefactor;
	if (gy < m_yOffset){
		y = (int)floor(local);
	}
	else{
		y = (int)ceil(local);
	}

	local = (gz - m_zOffset)/m_zScalefactor;
	if (gz < m_zOffset){
		z = (int)floor(local);
	}
	else{
		z = (int)ceil(local);
	}
}

void CWuLasLib::UpdateHeader()
{

}

/*
void CWuLasLib::Close()
{
	if (m_bOpenMode){
		if (m_flag == modeWrite){
			if (m_pLasMode == NULL){
				fclose(m_pfpFile);
				return;
			}
			else{
				UpdateRange();
				// useless
				// update the header
				((LASheader *)m_pHeader)->max_x = m_xMax;
				((LASheader *)m_pHeader)->min_x = m_xMin;
				((LASheader *)m_pHeader)->max_y = m_yMax;
				((LASheader *)m_pHeader)->min_y = m_yMin;
				((LASheader *)m_pHeader)->max_z = m_zMax;
				((LASheader *)m_pHeader)->min_z = m_zMin;

				if (m_nPoint < __INT32_MAX__){
					((LASheader *)m_pHeader)->number_of_point_records = m_nPoint;
				}
				else{
					((LASheader *)m_pHeader)->number_of_point_records = 0;
					((LASheader *)m_pHeader)->extended_number_of_point_records = m_nPoint;
				}

				((LASwriterLAS *)m_pLasMode)->close();
				fclose(m_pfpFile);
			}

			m_pfpFile = fopen(m_szLasPath, "rb+");

			// only rewrite the header can handle the point number is larger than INT32
			// lastool will check the number itself
			//fseek(m_pfpFile, 107, SEEK_SET);
			//fwrite(& m_nPoint, sizeof(unsigned int), 5, m_pfpFile);

			fseek(m_pfpFile, 179, SEEK_SET);
			fwrite(& m_xMax, sizeof(double), 1, m_pfpFile);
			fwrite(& m_xMin, sizeof(double), 1, m_pfpFile);
			fwrite(& m_yMax, sizeof(double), 1, m_pfpFile);
			fwrite(& m_yMin, sizeof(double), 1, m_pfpFile);
			fwrite(& m_zMax, sizeof(double), 1, m_pfpFile);
			fwrite(& m_zMin, sizeof(double), 1, m_pfpFile);
			fclose(m_pfpFile);

			m_bOpenMode = false;

			delete (LASwriterLAS *)m_pLasMode;		m_pLasMode = NULL;
			delete (LASheader *)m_pHeader;			m_pHeader = NULL;
		}
		else {
			((LASreaderLAS *)m_pLasMode)->close();
			fclose(m_pfpFile);
			m_bOpenMode = false;

			delete (LASreaderLAS *)m_pLasMode;		m_pLasMode = NULL;
		}
	}
}
*/

void CWuLasLib::Close()
{
	if (m_bOpenMode){
		if (m_flag == modeWrite){
			if (m_pOpener != NULL){
				((LASwriterLAS *)m_pLasMode)->close();
				//delete (LASwriterLAS *)m_pLasMode;		m_pLasMode = NULL;
				delete (LASwriteOpener *)m_pOpener; 	m_pOpener = NULL;
			}
		}
		else{
			if (m_pOpener != NULL){
				((LASreaderLAS *)m_pLasMode)->close();
				m_bOpenMode = false;
				//delete (LASreaderLAS *)m_pLasMode;		m_pLasMode = NULL;
				delete (LASreadOpener *)m_pOpener; 		m_pOpener = NULL;
			}
		}
	}
}
void CWuLasLib::UpdateRange()
{
	double local;
	
	local = (m_xMin - m_xOffset)/m_xScalefactor;
	if (m_xMin < m_xOffset){
		m_xMin = floor(local);
	}
	else{
		m_xMin = ceil(local);
	}
	m_xMin = m_xOffset + m_xMin * m_xScalefactor;

	local = (m_xMax - m_xOffset)/m_xScalefactor;
	if (m_xMax < m_xOffset){
		m_xMax = floor(local);
	}
	else{
		m_xMax = ceil(local);
	}
	m_xMax = m_xOffset + m_xMax * m_xScalefactor;
	
	local = (m_yMin - m_yOffset)/m_yScalefactor;
	if (m_yMin < m_yOffset){
		m_yMin = (int)floor(local);
	}
	else{
		m_yMin = (int)ceil(local);
	}
	m_yMin = m_yOffset + m_yMin * m_yScalefactor;

	local = (m_yMax - m_yOffset)/m_yScalefactor;
	if (m_yMax < m_yOffset){
		m_yMax = (int)floor(local);
	}
	else{
		m_yMax = (int)ceil(local);
	}
	m_yMax = m_yOffset + m_yMax * m_yScalefactor;
	
	local = (m_zMin - m_zOffset)/m_zScalefactor;
	if (m_zMin < m_zOffset){
		m_zMin = floor(local);
	}
	else{
		m_zMin = ceil(local);
	}
	m_zMin = m_zOffset + m_zMin * m_zScalefactor;

	local = (m_zMax - m_zOffset)/m_zScalefactor;
	if (m_zMax < m_zOffset){
		m_zMax = floor(local);
	}
	else{
		m_zMax = ceil(local);
	}
	m_zMax = m_zOffset + m_zMax * m_zScalefactor;
	
	/*std::cout << "xmin xmax : " << m_xMin << "\t" << m_xMax << std::endl;
	std::cout << "ymin ymax : " << m_yMin << "\t" << m_yMax << std::endl;
	std::cout << "zmin zmax : " << m_zMin << "\t" << m_zMax << std::endl;*/
}

int64_t CWuLasLib::GetPtNum()
{
	/*LASheader * pHeader = &(((LASreaderLAS *)m_pLasMode)->header);
	return pHeader->number_of_point_records + pHeader->extended_number_of_point_records;*/
	return m_nPoint;
}

void CWuLasLib::GetRange(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	xmin = m_xMin;
	xmax = m_xMax;
	
	ymin = m_yMin;
	ymax = m_yMax;
	
	zmin = m_zMin;
	zmax = m_zMax;
}

void CWuLasLib::GetOffset( double & xOffset, double & yOffset, double & zOffset )
{
	xOffset = m_xOffset;
	yOffset = m_yOffset;
	zOffset = m_zOffset;
}

void CWuLasLib::GetPrecision( double & xPrecision, double & yPrecision, double & zPrecision )
{
	xPrecision = m_xScalefactor;
	yPrecision = m_yScalefactor;
	zPrecision = m_zScalefactor;
}

void CWuLasLib::ReadLasHeader()
{
	LASheader * pHeader = &(((LASreader *)m_pLasMode)->header);

	m_xScalefactor = pHeader->x_scale_factor;
	m_yScalefactor = pHeader->y_scale_factor;
	m_zScalefactor = pHeader->z_scale_factor;

	m_xOffset = pHeader->x_offset;
	m_yOffset = pHeader->y_offset;
	m_zOffset = pHeader->z_offset;

	m_nPoint = pHeader->number_of_point_records + pHeader->extended_number_of_point_records;
	
	m_xMin = pHeader->min_x;
	m_xMax = pHeader->max_x;
	
	m_yMin = pHeader->min_y;
	m_yMax = pHeader->max_y;
	
	m_zMin = pHeader->min_z;
	m_zMax = pHeader->max_z;
}

void CWuLasLib::Local2GlobalCoord( int x, int y, int z, double & gx, double & gy, double & gz )
{
	gx = x * m_xScalefactor + m_xOffset;
	gy = y * m_yScalefactor + m_yOffset;
	gz = z * m_zScalefactor + m_zOffset;
}

bool CWuLasLib::ReadLas( LasPoint * pLasPoint, int nListSize )
{
	int i;

	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	for (i = 0; i < nListSize; ++i, ++pLasPoint){
		pLasReader->read_point();
		Local2GlobalCoord(pLasReader->point.X, pLasReader->point.Y, pLasReader->point.Z, pLasPoint->x, pLasPoint->y, pLasPoint->z);
		//memcpy(pLasPoint->rgb, pLasReader->point.rgb, sizeof(short) * 3);
		for (int j = 0; j < 3; ++j){
			pLasPoint->rgb[j] = pLasReader->point.rgb[j];
		}
	}

	return true;
}

bool CWuLasLib::ReadLas( LasPointClass * pLasPoint, int nListSize )
{
	int i;
	
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	for (i = 0; i < nListSize; ++i, ++pLasPoint){
		pLasReader->read_point();
		Local2GlobalCoord(pLasReader->point.X, pLasReader->point.Y, pLasReader->point.Z, pLasPoint->x, pLasPoint->y, pLasPoint->z);
		pLasPoint->intensity      = pLasReader->point.intensity;
		pLasPoint->classification = pLasReader->point.classification;
		//memcpy(pLasPoint->rgb, pLasReader->rgb, sizeof(short) * 3);
		for (int j = 0; j < 3; ++j){
			pLasPoint->rgb[j] = pLasReader->point.rgb[j];
		}
	}
	
	return true;
}

bool CWuLasLib::ReadLas( LasPointFull * pLasPoint, int nListSize )
{
	int i;
	
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	for (i = 0; i < nListSize; ++i, ++pLasPoint){
		pLasReader->read_point();
		Local2GlobalCoord(pLasReader->point.X, pLasReader->point.Y, pLasReader->point.Z, pLasPoint->x, pLasPoint->y, pLasPoint->z);
		pLasPoint->intensity       = pLasReader->point.intensity;
		pLasPoint->classification  = pLasReader->point.classification;
		pLasPoint->point_source_ID = pLasReader->point.point_source_ID;
		//memcpy(pLasPoint->rgb, pLasReader->rgb, sizeof(short) * 3);
		for (int j = 0; j < 3; ++j){
			pLasPoint->rgb[j] = pLasReader->point.rgb[j];
		}
	}
	
	return true;
}

bool CWuLasLib::ReadLas( LasPointEcho * pLasPoint, int nListSize )
{
	int i;

	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	for (i = 0; i < nListSize; ++i, ++pLasPoint){
		pLasReader->read_point();
		Local2GlobalCoord(pLasReader->point.X, pLasReader->point.Y, pLasReader->point.Z, pLasPoint->x, pLasPoint->y, pLasPoint->z);
		pLasPoint->intensity       = pLasReader->point.intensity;
		pLasPoint->classification  = pLasReader->point.classification;
		pLasPoint->echo            = pLasReader->point.return_number;
		pLasPoint->sumechos        = pLasReader->point.number_of_returns;
		//memcpy(pLasPoint->rgb, pLasReader->rgb, sizeof(short) * 3);
		for (int j = 0; j < 3; ++j){
			pLasPoint->rgb[j] = pLasReader->point.rgb[j];
		}
	}

	return true;
}

bool CWuLasLib::ReadLas( DPT3D * pLasPoint, int64_t nListSize )
{
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	for (int64_t i = 0; i < nListSize; ++i, ++pLasPoint){
		pLasReader->read_point();
		Local2GlobalCoord(pLasReader->point.X, pLasReader->point.Y, pLasReader->point.Z, pLasPoint->x, pLasPoint->y, pLasPoint->z);
	}
	
	return true;
}

bool CWuLasLib::ReadLas( int nStartIndex, LasPoint * pLasPoint, int nListSize )
{
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;

	pLasReader->seek(nStartIndex);
	
	ReadLas(pLasPoint, nListSize);

	return true;
}

bool CWuLasLib::ReadLas( int nStartIndex, LasPointClass * pLasPoint, int nListSize )
{
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	
	pLasReader->seek(nStartIndex);
	
	ReadLas(pLasPoint, nListSize);
	
	return true;
}

bool CWuLasLib::ReadLas( int nStartIndex, LasPointFull * pLasPoint, int nListSize )
{
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	
	pLasReader->seek(nStartIndex);
	
	ReadLas(pLasPoint, nListSize);
	
	return true;
}

bool CWuLasLib::ReadLas( int nStartIndex, DPT3D * pLasPoint, int nListSize )
{
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	
	pLasReader->seek(nStartIndex);
	
	ReadLas(pLasPoint, nListSize);
	
	return true;
}

bool CWuLasLib::ReadLas( int nStartIndex, LasPointEcho * pLasPoint, int nListSize )
{
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;

	pLasReader->seek(nStartIndex);

	ReadLas(pLasPoint, nListSize);

	return true;
}
