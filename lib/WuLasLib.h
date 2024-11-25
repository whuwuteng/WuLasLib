// WuLasLib.h : main header file for the WULASLIB DLL
//
//////////////////////////////////////////////////////////////////////
#ifndef WULASLIB_H__WHU_WUTENG_2014_05_16_14_40_03
#define WULASLIB_H__WHU_WUTENG_2014_05_16_14_40_03

#pragma once

#include <stdio.h>
#include <stdlib.h>

// Definition of LasPoint structure
#ifndef LASPOINT_ITEM
#define LASPOINT_ITEM
typedef struct tagLasPoint{
	double				x;						// x coordinate
	double				y;						// y coordinate
	double				z;						// z coordinate
	unsigned short		rgb[3];					// RGB color
}LasPoint;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_ITEM already defined. Ensure it matches: struct tagLasPoint{ double x,y,z; unsigned short rgb[3]; }.")
#endif

// Definition of LasPointClass structure
#ifndef LASPOINT_CLASS
#define LASPOINT_CLASS
typedef struct tagLasPointClass{
	double				x;						// x coordinate
	double				y;						// y coordinate
	double				z;						// z coordinate
	unsigned short		rgb[3];					// RGB color
	unsigned short		intensity;				// Intensity information
	unsigned char		classification;			// Classification information
}LasPointClass;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_CLASS already defined. Ensure it matches: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; }.")
#endif

// Definition of LasPointFull structure
#ifndef LASPOINT_FULL
#define LASPOINT_FULL
typedef struct tagLasPointFull{
	double				x;						// x coordinate
	double				y;						// y coordinate
	double				z;						// z coordinate
	unsigned short		rgb[3];					// RGB color
	unsigned short		intensity;				// Intensity information
	unsigned char		classification;			// Classification information
	unsigned short		point_source_ID;		// Point ID value
}LasPointFull;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_FULL already defined. Ensure it matches: struct tagLasPointFull{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned short point_source_ID; }.")
#endif

// Definition of LasPointEcho structure
#ifndef LASPOINT_ECHO
#define LASPOINT_ECHO
typedef struct tagLasPointEcho{
	double				x;						// x coordinate
	double				y;						// y coordinate
	double				z;						// z coordinate
	unsigned short		rgb[3];					// RGB color
	unsigned short		intensity;				// Intensity information
	unsigned char		classification;			// Classification information
	unsigned char       echo;					// nth echo
	unsigned char       sumechos;				// Total number of echoes
}LasPointEcho;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_ECHO already defined. Ensure it matches: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned char echo; unsigned char sumechos; }.")
#endif

// Definition of LasPointTime structure
#ifndef LASPOINT_TIME
#define LASPOINT_TIME
typedef struct tagLasPointTime{
	double				x;						// x coordinate
	double				y;						// y coordinate
	double				z;						// z coordinate
	unsigned short		rgb[3];					// RGB color
	unsigned short		intensity;				// Intensity information
	unsigned char       echo;					// nth echo
	unsigned char       sumechos;				// Total number of echoes
	double              gpstime;                // GPS time
	unsigned char		scanflag;				// Scan direction flag
	unsigned char		scanline;				// Edge of flight line
}LasPointTime;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_TIME already defined. Ensure it matches: struct tagLasPointTime{ double x,y,z; unsigned short rgb[3]; unsigned char echo; unsigned char sumechos; double gpstime; }.")
#endif

// Definition of LasPointUltra structure
#ifndef LASPOINT_ULTRA
#define LASPOINT_ULTRA
typedef struct tagLasPointUltra{
	double				x;						// x coordinate
	double				y;						// y coordinate
	double				z;						// z coordinate
	unsigned short		rgb[3];					// RGB color
	unsigned short		intensity;				// Intensity information
	unsigned char		classification;			// Classification information
	unsigned char       echo;					// nth echo
	unsigned char       sumechos;				// Total number of echoes
	double              gpstime;                // GPS time
	unsigned char		scanflag;				// Scan direction flag
	unsigned char		scanline;				// Edge of flight line
}LasPointUltra;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_ULTRA already defined. Ensure it matches: struct tagLasPointUltra{ double x,y,z; unsigned short rgb[3]; unsigned char echo; unsigned char sumechos; double gpstime; }.")
#endif

// Definition of DPT3D structure
#ifndef _DPT3D
#define _DPT3D
typedef struct tagDPT3D
{
	double				x;						// x coordinate
	double				y;						// y coordinate
	double				z;						// z coordinate
}DPT3D;
#else
#pragma message("WuLasLib.h, Warning: DPT3D already defined. Ensure it matches: struct tagDPT3D{ double x,y,z; }.")
#endif

// Implementation of the CWuLasLib class
class CWuLasLib
{
public:
    CWuLasLib();
    virtual ~CWuLasLib();

    //! File access modes
    enum OPENFLAGS { 
        modeRead = 1,  // Open in read-only mode
        modeWrite      // Open in write mode
    };

public:
    // Open operation
    // TODO: Add LAZ support
    bool Open(const char* pszLasFile, OPENFLAGS flag = modeRead);

    // Close operation
    void Close();

    // Get error message
    const char* GetLastError();

    /********************************************************************************************************/
    // Write operations (writing must use the same mode)
    
    // Set offset values
    void SetOffset(double xOffset, double yOffset, double zOffset);

    // Set precision (specify the number of decimal places to retain)
    void SetPrecision(double xPrecision, double yPrecision, double zPrecision);

    // Write point cloud (append to the file from the last written position)
    bool WriteLas(LasPoint* pLasPoint, int nPoint);
    bool WriteLas(LasPointClass* pLasPoint, int nPoint);
    bool WriteLas(LasPointFull* pLasPoint, int nPoint);
    bool WriteLas(LasPointEcho* pLasPoint, int nPoint);
    bool WriteLas(LasPointTime* pLasPoint, int nPoint);
    bool WriteLas(LasPointUltra* pLasPoint, int nPoint);
    
    // Write only the point cloud coordinates (append to the file from the last written position)
    bool WriteLas(DPT3D* pLasPoint, int nPoint);

    // Write point cloud (insert at a specified position)
    bool WriteLas(int nStartIndex, LasPoint* pLasPoint, int nListSize);
    bool WriteLas(int nStartIndex, LasPointClass* pLasPoint, int nListSize);
    bool WriteLas(int nStartIndex, LasPointFull* pLasPoint, int nListSize);
    bool WriteLas(int nStartIndex, LasPointEcho* pLasPoint, int nListSize);
    bool WriteLas(int nStartIndex, LasPointTime* pLasPoint, int nListSize);
    bool WriteLas(int nStartIndex, LasPointUltra* pLasPoint, int nListSize);

    // Write only the point cloud coordinates (insert at a specified position)
    bool WriteLas(int nStartIndex, DPT3D* pLasPoint, int nListSize);

    // Read operations
    
    // Get the number of points
    int GetPtNum();

    // Get the range
    void GetRange(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax);

    // Get offset values
    void GetOffset(double& xOffset, double& yOffset, double& zOffset);

    // Get precision (number of decimal places retained for points)
    void GetPrecision(double& xPrecision, double& yPrecision, double& zPrecision);

    // Read point cloud (read from the last read position)
    bool ReadLas(LasPoint* pLasPoint, int nListSize);
    bool ReadLas(LasPointClass* pLasPoint, int nListSize);
    bool ReadLas(LasPointFull* pLasPoint, int nListSize);
    bool ReadLas(LasPointEcho* pLasPoint, int nListSize);
    bool ReadLas(LasPointTime* pLasPoint, int nListSize);
    bool ReadLas(LasPointUltra* pLasPoint, int nListSize);

    // Read only the point cloud coordinates (read from the last read position)
    bool ReadLas(DPT3D* pLasPoint, int nListSize);

    // Read point cloud (read from a specified position)
    bool ReadLas(int nStartIndex, LasPoint* pLasPoint, int nListSize);
    bool ReadLas(int nStartIndex, LasPointClass* pLasPoint, int nListSize);
    bool ReadLas(int nStartIndex, LasPointFull* pLasPoint, int nListSize);
    bool ReadLas(int nStartIndex, LasPointEcho* pLasPoint, int nListSize);
    bool ReadLas(int nStartIndex, LasPointTime* pLasPoint, int nListSize);
    bool ReadLas(int nStartIndex, LasPointUltra* pLasPoint, int nListSize);

    // Read only the point cloud coordinates (read from a specified position)
    bool ReadLas(int nStartIndex, DPT3D* pLasPoint, int nListSize);

    /********************************************************************************************************/

protected:
    // Error message
    char m_szErrorMsg[512];

    // File path
    char m_szLasPath[512];

    // GPS Time Type
    unsigned char m_GPSType;

    // Offset values
    double m_xOffset;
    double m_yOffset;
    double m_zOffset;

    // Scaling factors (used for precision when writing)
    double m_xScalefactor;
    double m_yScalefactor;
    double m_zScalefactor;

    // File mode flag
    OPENFLAGS m_flag;

    // File pointer
    FILE* m_pfpFile;

    // LAS mode
    void* m_pLasMode;

    // Additional fields for writing
    void* m_pHeader;
    bool m_bOpenMode;

    // Point cloud range
    double m_xMax;
    double m_xMin;
    double m_yMax;
    double m_yMin;
    double m_zMax;
    double m_zMin;

    // Number of points
    int m_nPoint;

protected:
    // Reset the internal state
    void Reset();

    // Write LAS header
    bool WriteLasHeader(bool bColor = true, bool bGPS = false);

    // Read LAS header
    void ReadLasHeader();

    // Transform coordinates using offset and scaling factors (global to local coordinate system)
    void Global2LocalCoord(double gx, double gy, double gz, int& x, int& y, int& z);

    // Transform coordinates using offset and scaling factors (local to global coordinate system)
    void Local2GlobalCoord(int x, int y, int z, double& gx, double& gy, double& gz);

    // Update boundary values
    void UpdateRange();
};
#endif // WULASLIB_H__WHU_WUTENG_2014_05_16_14_40_03
