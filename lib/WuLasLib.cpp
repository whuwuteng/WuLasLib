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

// lasTool is not read in one go, so it cannot be used.
const int MEMORY_SIZE = 100 * 1024 * 1024;

CWuLasLib::CWuLasLib()
{
    // Initialization
    memset(m_szErrorMsg, 0, sizeof(char) * 512);
    memset(m_szLasPath,  0, sizeof(char) * 512);

    m_xOffset = 0;
    m_yOffset = 0;
    m_zOffset = 0;

    m_xScalefactor = 1.0;
    m_yScalefactor = 1.0;
    m_zScalefactor = 1.0;

    // File flags
    m_pfpFile   = NULL;
    m_pLasMode  = NULL;
    m_bOpenMode = false;

    // Initialize bounds
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
    // Initialization
    memset(m_szErrorMsg, 0, sizeof(char) * 512);
    memset(m_szLasPath,  0, sizeof(char) * 512);

    m_xOffset = 0;
    m_yOffset = 0;
    m_zOffset = 0;

    m_xScalefactor = 1.0;
    m_yScalefactor = 1.0;
    m_zScalefactor = 1.0;

    if (m_bOpenMode) {
        Close();
    } else {
        m_pfpFile  = NULL;
        m_pLasMode = NULL;
    }

    // Initialize bounds
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

bool CWuLasLib::Open(const char * pszLasFile, OPENFLAGS flag /*= modeRead*/)
{
    Reset();

    strcpy(m_szLasPath, pszLasFile);
    m_flag = flag;

    // File operations
    if (m_flag == modeRead) {
        m_pfpFile = fopen(pszLasFile, "rb");
        if (m_pfpFile == NULL) {
            sprintf(m_szErrorMsg, "%s cannot be opened!\n", pszLasFile);
            return false;
        }

        m_pLasMode = new LASreaderLAS();
        bool bOpenSuccess = ((LASreaderLAS *)m_pLasMode)->open(m_pfpFile);

        if (!bOpenSuccess) {
            strcpy(m_szErrorMsg, "Cannot read LAS file header!");
            delete m_pLasMode;              m_pLasMode = NULL;
            fclose(m_pfpFile);              m_pfpFile  = NULL;
            return false;
        }

        ReadLasHeader();
    } else {
        m_pfpFile = fopen(pszLasFile, "wb");
        if (m_pfpFile == NULL) {
            sprintf(m_szErrorMsg, "%s cannot be opened!\n", pszLasFile);
            return false;
        }
    }
    m_bOpenMode = true;

    return true;
}

void CWuLasLib::SetOffset(double xOffset, double yOffset, double zOffset)
{
    m_xOffset = xOffset;
    m_yOffset = yOffset;
    m_zOffset = zOffset;
}

void CWuLasLib::SetPrecision(double xPrecision, double yPrecision, double zPrecision)
{
    m_xScalefactor = xPrecision;
    m_yScalefactor = yPrecision;
    m_zScalefactor = zPrecision;
}

// Header settings for write mode only
bool CWuLasLib::WriteLasHeader(bool bColor /*= true*/, bool bGPS /*= false*/)
{
    m_pHeader = new LASheader();
    LASheader * pHeader = (LASheader *)m_pHeader;

    pHeader->version_major = 1;
    pHeader->version_minor = 2;

    if (bGPS) {
        if (bColor) {
            pHeader->point_data_format = 3;
            pHeader->point_data_record_length = 34;
        } else {
            pHeader->point_data_format = 1;
            pHeader->point_data_record_length = 28;
        }
    } else {
        if (bColor) {
            pHeader->point_data_format = 2;
            pHeader->point_data_record_length = 26;
        } else {
            pHeader->point_data_format = 0;
            pHeader->point_data_record_length = 20;
        }
    }

    strcpy(pHeader->system_identifier, "LasTool");
    strcpy(pHeader->generating_software, "WuLasLib");

    time_t long_time;
    time(&long_time);                
    struct tm * pTime = localtime(&long_time); 

    pHeader->file_creation_year = pTime->tm_year + 1900;
    pHeader->file_creation_day  = pTime->tm_yday;

    pHeader->x_scale_factor = m_xScalefactor;
    pHeader->y_scale_factor = m_yScalefactor;
    pHeader->z_scale_factor = m_zScalefactor;

    pHeader->x_offset = m_xOffset;
    pHeader->y_offset = m_yOffset;
    pHeader->z_offset = m_zOffset;

    m_pLasMode = new LASwriterLAS();
    bool bOpenSuccess = ((LASwriterLAS *)m_pLasMode)->open(m_pfpFile, pHeader, 0);

    if (!bOpenSuccess) {
        strcpy(m_szErrorMsg, "Cannot write LAS file header!");
        delete m_pLasMode;              m_pLasMode = NULL;
    }

    return bOpenSuccess;
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

	// Write point cloud file
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
	
	// Write point cloud file
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
	
	// Write point cloud file
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
		pLasWriter->write_point(& pt);
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

	// Write point cloud file
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

bool CWuLasLib::WriteLas(LasPointTime* pLasPoint, int nPoint)
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
		if (! WriteLasHeader(true, true) ){
			return false;
		}
	}

	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;

	// Write point cloud file
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
		pt.return_number   = pLasPoint->echo;
		pt.number_of_returns = pLasPoint->sumechos;
		pt.have_gps_time = true;
		pt.gps_time = pLasPoint->gpstime;
		//pLasWriter->write_point(& pt, 0, pLasPoint->rgb);
		for (int j = 0; j < 3; ++j){
			pt.rgb[j] = pLasPoint->rgb[j];
		}
		pt.scan_direction_flag = pLasPoint->scanflag;
		pt.edge_of_flight_line = pLasPoint->scanline;
		pLasWriter->write_point(& pt);
		++m_nPoint;
	}

	return true;
}

bool CWuLasLib::WriteLas(LasPointUltra * pLasPoint, int nPoint)
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
		if (! WriteLasHeader(true, true) ){
			return false;
		}
	}

	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;

	// Write point cloud file
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
		pt.have_gps_time = true;
		pt.gps_time = pLasPoint->gpstime;
		//pLasWriter->write_point(& pt, 0, pLasPoint->rgb);
		for (int j = 0; j < 3; ++j){
			pt.rgb[j] = pLasPoint->rgb[j];
		}
		pt.scan_direction_flag = pLasPoint->scanflag;
		pt.edge_of_flight_line = pLasPoint->scanline;
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
	
	// Write point cloud file
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

bool CWuLasLib::WriteLas( int nStartIndex, LasPointTime * pLasPoint, int nListSize )
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
		if (! WriteLasHeader(true, true) ){
			return false;
		}
	}

	LASwriterLAS * pLasWriter = (LASwriterLAS *)m_pLasMode;

	pLasWriter->Offset(nStartIndex);

	WriteLas(pLasPoint, nListSize);

	return true;
}

bool CWuLasLib::WriteLas( int nStartIndex, LasPointUltra * pLasPoint, int nListSize )
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
		if (! WriteLasHeader(true, true) ){
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

void CWuLasLib::Close()
{
	if (m_bOpenMode){
		if (m_flag == modeWrite){
			if (m_pLasMode == NULL){
				fclose(m_pfpFile);
				return;
			}
			else{
				((LASwriterLAS *)m_pLasMode)->close();
				fclose(m_pfpFile);
			}

			UpdateRange();

			m_pfpFile = fopen(m_szLasPath, "rb+");
			fseek(m_pfpFile, 107, SEEK_SET);
			fwrite(& m_nPoint, sizeof(unsigned int), 5, m_pfpFile);
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

int CWuLasLib::GetPtNum()
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

	// GPS Time Type
	m_GPSType = pHeader->global_encoding%2;

	// debug
	// The meaning of GPS Time in the point records. If this bit is not set, the GPS time in the point record fields is GPS Week Time (the same as versions 1.0 through 1.2 of LAS). Otherwise, if this bit is set, the GPS Time is standard GPS Time (satellite GPS Time) minus 1 x 109 (Adjusted Standard GPS Time). The offset moves the time back to near zero to improve floating point resolution. The origin of standard GPS Time is defined as midnight of the morning of January 6, 1980.

	//printf("GPS Time Type = %d\n", int(m_GPSType));

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

bool CWuLasLib::ReadLas( LasPointTime * pLasPoint, int nListSize )
{
	int i;

	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	for (i = 0; i < nListSize; ++i, ++pLasPoint){
		pLasReader->read_point();
		Local2GlobalCoord(pLasReader->point.X, pLasReader->point.Y, pLasReader->point.Z, pLasPoint->x, pLasPoint->y, pLasPoint->z);
		pLasPoint->intensity       = pLasReader->point.intensity;
		pLasPoint->echo            = pLasReader->point.return_number;
		pLasPoint->sumechos		   = pLasReader->point.number_of_returns;
		if (pLasReader->point.have_gps_time){
			pLasPoint->gpstime = pLasReader->point.gps_time;
		}
		pLasPoint->scanflag = pLasReader->point.scan_direction_flag;
        pLasPoint->scanline = pLasReader->point.edge_of_flight_line;
		//memcpy(pLasPoint->rgb, pLasReader->rgb, sizeof(short) * 3);
		for (int j = 0; j < 3; ++j){
			pLasPoint->rgb[j] = pLasReader->point.rgb[j];
		}
	}

	return true;
}

bool CWuLasLib::ReadLas( LasPointUltra * pLasPoint, int nListSize )
{
	int i;

	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	for (i = 0; i < nListSize; ++i, ++pLasPoint){
		pLasReader->read_point();
		Local2GlobalCoord(pLasReader->point.X, pLasReader->point.Y, pLasReader->point.Z, pLasPoint->x, pLasPoint->y, pLasPoint->z);
		pLasPoint->intensity       = pLasReader->point.intensity;
		pLasPoint->classification  = pLasReader->point.classification;
		pLasPoint->echo            = pLasReader->point.return_number;
		pLasPoint->sumechos		   = pLasReader->point.number_of_returns;
		if (pLasReader->point.have_gps_time){
			pLasPoint->gpstime = pLasReader->point.gps_time;
		}
		pLasPoint->scanflag = pLasReader->point.scan_direction_flag;
        pLasPoint->scanline = pLasReader->point.edge_of_flight_line;
		//memcpy(pLasPoint->rgb, pLasReader->rgb, sizeof(short) * 3);
		for (int j = 0; j < 3; ++j){
			pLasPoint->rgb[j] = pLasReader->point.rgb[j];
		}
	}

	return true;
}

bool CWuLasLib::ReadLas( DPT3D * pLasPoint, int nListSize )
{
	int i;
	
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;
	for (i = 0; i < nListSize; ++i, ++pLasPoint){
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

bool CWuLasLib::ReadLas( int nStartIndex, LasPointTime * pLasPoint, int nListSize )
{
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;

	pLasReader->seek(nStartIndex);

	ReadLas(pLasPoint, nListSize);

	return true;
}

bool CWuLasLib::ReadLas( int nStartIndex, LasPointUltra * pLasPoint, int nListSize )
{
	LASreaderLAS * pLasReader = (LASreaderLAS *)m_pLasMode;

	pLasReader->seek(nStartIndex);

	ReadLas(pLasPoint, nListSize);

	return true;
}
