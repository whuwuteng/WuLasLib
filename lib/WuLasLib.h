// WuLasLib.h : main header file for the WULASLIB DLL
//
//////////////////////////////////////////////////////////////////////
#ifndef WULASLIB_H__WHU_WUTENG_2014_05_16_14_40_03
#define WULASLIB_H__WHU_WUTENG_2014_05_16_14_40_03

#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifndef LASPOINT_ITEM
#define LASPOINT_ITEM
typedef struct tagLasPoint{
	double				x;						// x ����	
	double				y;						// y ����
	double				z;						// z ����
	unsigned short		rgb[3];					// RGB��ɫ
}LasPoint;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_ITEM alread define,be sure it was define as: struct tagLasPoint{ double x,y,z; unsigned short rgb[3]; }. \
               \nWuLasLib.h, ����:���� LASPOINT_ITEM �Ѿ������,��ȷ���䶨��Ϊ: struct tagLasPoint{ double x,y,z; unsigned short rgb[3];}")
#endif

#ifndef LASPOINT_CLASS
#define LASPOINT_CLASS
typedef struct tagLasPointClass{
	double				x;						// x ����	
	double				y;						// y ����
	double				z;						// z ����
	unsigned short		rgb[3];					// RGB��ɫ
	unsigned short		intensity;				// ǿ����Ϣ
	unsigned char		classification;			// �����Ϣ
}LasPointClass;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_CLASS alread define,be sure it was define as: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification;}. \
			   \nWuLasLib.h, ����:���� LASPOINT_CLASS �Ѿ������,��ȷ���䶨��Ϊ: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification;}")
#endif

#ifndef LASPOINT_FULL
#define LASPOINT_FULL
typedef struct tagLasPointFull{
	double				x;						// x ����	
	double				y;						// y ����
	double				z;						// z ����
	unsigned short		rgb[3];					// RGB��ɫ
	unsigned short		intensity;				// ǿ����Ϣ
	unsigned char		classification;			// �����Ϣ
	unsigned short		point_source_ID;		// ���IDֵ
}LasPointFull;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_FULL alread define,be sure it was define as: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned short point_source_ID;}. \
				\nWuLasLib.h, ����:���� LASPOINT_FULL �Ѿ������,��ȷ���䶨��Ϊ: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned short point_source_ID;}")
#endif

#ifndef LASPOINT_ECHO
#define LASPOINT_ECHO
typedef struct tagLasPointEcho{
	double				x;						// x ����	
	double				y;						// y ����
	double				z;						// z ����
	unsigned short		rgb[3];					// RGB��ɫ
	unsigned short		intensity;				// ǿ����Ϣ
	unsigned char		classification;			// �����Ϣ
	unsigned char       echo;					// ��n�λز�
	unsigned char       sumechos;				// �ܻز�����
}LasPointEcho;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_ECHO alread define,be sure it was define as: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned char echo; unsigned char sumechos;}. \
			   \nWuLasLib.h, ����:���� LASPOINT_ECHO �Ѿ������,��ȷ���䶨��Ϊ: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned char echo; unsigned char sumechos;}")
#endif

#ifndef LASPOINT_TIME
#define LASPOINT_TIME
typedef struct tagLasPointTime{
	double				x;						// x ����	
	double				y;						// y ����
	double				z;						// z ����
	unsigned short		rgb[3];					// RGB��ɫ
	unsigned char       echo;					// ��n�λز�
	unsigned char       sumechos;				// �ܻز�����
	double              gpstime;                // GPS time
}LasPointTime;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_TIME alread define,be sure it was define as: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned char echo; unsigned char sumechos; double gpstime;}. \
			   \nWuLasLib.h, ����:���� LASPOINT_TIME �Ѿ������,��ȷ���䶨��Ϊ: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned char echo; unsigned char sumechos; double gpstime;}")
#endif

#ifndef _DPT3D
#define _DPT3D
typedef struct tagDPT3D
{
	double				x;						// x ����	
	double				y;						// y ����
	double				z;						// z ����
}DPT3D;
#else
#pragma  message("WuLasLib.h, Warning: DPT3D alread define,be sure it was define as: struct tagDPT3D{ double x,y,z; }. \
				\nWuLasLib.h, ����:���� DPT3D �Ѿ������,��ȷ���䶨��Ϊ: struct tagDPT3D{ double x,y,z; }") 
#endif

// ��ʵ��д�Ĺ���
class CWuLasLib
{
public:
	CWuLasLib();
	virtual ~CWuLasLib();

	//! �ļ�����ģʽ
	enum OPENFLAGS{ modeRead = 1,						// ��ֻ���ķ�ʽ��				
					modeWrite							// �Կ�д�ķ�ʽ��
	};

public:
	// �򿪲���
	bool						Open(const char * pszLasFile, OPENFLAGS flag = modeRead);
	// �رղ���
	void						Close();
	// ��ô�����Ϣ
	const char *				GetLastError();
/********************************************************************************************************/
	// д�ļ��Ĳ���,д�ļ�ֻ����ͬһ��ģʽ
	// ����ƫ����
	void						SetOffset(double xOffset, double yOffset, double zOffset);
	// ���þ���,�������ı��浽С�����λ
	void						SetPrecision(double xPrecision, double yPrecision, double zPrecision);
	// д����(���ϴεĵط���ʼд��)
	bool						WriteLas(LasPoint * pLasPoint, int nPoint);
	// д����(���ϴεĵط���ʼд��)
	bool						WriteLas(LasPointClass * pLasPoint, int nPoint);
	// д����(���ϴεĵط���ʼд��)
	bool						WriteLas(LasPointFull * pLasPoint, int nPoint);
	// д����(���ϴεĵط���ʼд��)
	bool						WriteLas(LasPointEcho * pLasPoint, int nPoint);
	// д����(���ϴεĵط���ʼд��)
	bool						WriteLas(LasPointTime * pLasPoint, int nPoint);
	// ֻд���Ƶ�����(���ϴεĵط���ʼд��)
	bool						WriteLas(DPT3D * pLasPoint, int nPoint);
	// д����(�м�д��)
	bool						WriteLas(int nStartIndex, LasPoint * pLasPoint, int nListSize);
	// д����(�м�д��)
	bool						WriteLas(int nStartIndex, LasPointClass * pLasPoint, int nListSize);
	// д����(�м�д��)
	bool						WriteLas(int nStartIndex, LasPointFull * pLasPoint, int nListSize);
	// д����(�м�д��)
	bool						WriteLas(int nStartIndex, LasPointEcho * pLasPoint, int nListSize);
	// д����(�м�д��)
	bool						WriteLas(int nStartIndex, LasPointTime * pLasPoint, int nListSize);
	// ֻд���Ƶ�����(�м�д��)
	bool						WriteLas(int nStartIndex, DPT3D * pLasPoint, int nListSize);

	// ���ļ��Ĳ���
	// ��õ�ĸ���
	int							GetPtNum();
	// ��÷�Χ
	void						GetRange(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax);
	// ���ƫ����
	void						GetOffset(double & xOffset, double & yOffset, double & zOffset);
	// ��þ���,�������ı��浽С�����λ
	void						GetPrecision(double & xPrecision, double & yPrecision, double & zPrecision);
	// ������(���ϴεĵط���ʼ��ȡ)
	bool						ReadLas(LasPoint * pLasPoint, int nListSize);
	// ������(���ϴεĵط���ʼ��ȡ)
	bool						ReadLas(LasPointClass * pLasPoint, int nListSize);
	// ������(���ϴεĵط���ʼ��ȡ)
	bool						ReadLas(LasPointFull * pLasPoint, int nListSize);
	// ������(���ϴεĵط���ʼ��ȡ)
	bool						ReadLas(LasPointEcho * pLasPoint, int nListSize);
	// ������(���ϴεĵط���ʼ��ȡ)
	bool						ReadLas(LasPointTime * pLasPoint, int nListSize);
	// ֻ��ȡ���Ƶ�����(���ϴεĵط���ʼ��ȡ)
	bool						ReadLas(DPT3D * pLasPoint, int nListSize);
	// ������(�м��ȡ)
	bool						ReadLas(int nStartIndex, LasPoint * pLasPoint, int nListSize);
	// ������(�м��ȡ)
	bool						ReadLas(int nStartIndex, LasPointClass * pLasPoint, int nListSize);
	// ������(�м��ȡ)
	bool						ReadLas(int nStartIndex, LasPointFull * pLasPoint, int nListSize);
	// ������(�м��ȡ)
	bool						ReadLas(int nStartIndex, LasPointEcho * pLasPoint, int nListSize);
	// ������(�м��ȡ)
	bool						ReadLas(int nStartIndex, LasPointTime * pLasPoint, int nListSize);
	// ֻ��ȡ���Ƶ�����(�м��ȡ)
	bool						ReadLas(int nStartIndex, DPT3D * pLasPoint, int nListSize);
/********************************************************************************************************/

protected:
	// ������Ϣ
	char						m_szErrorMsg[512];
	// �ļ���·��
	char						m_szLasPath[512];
	// ƫ����
	double						m_xOffset;
	double						m_yOffset;
	double						m_zOffset;
	// ����ϵ��,д��ʱ����ݾ�����
	double						m_xScalefactor;
	double						m_yScalefactor;
	double						m_zScalefactor;
	// �ļ���־
	OPENFLAGS					m_flag;
	FILE *						m_pfpFile;
	void *						m_pLasMode;
	// add for the write(only)
	void * 						m_pHeader;
	bool						m_bOpenMode;
	// ��ķ�Χ
	double						m_xMax;
	double						m_xMin;
	double						m_yMax;
	double						m_yMin;
	double						m_zMax;
	double						m_zMin;
	// ��ĸ���
	int							m_nPoint;

protected:
	void						Reset();
	// дLas��ͷ�ļ�
	bool						WriteLasHeader(bool bColor = true, bool bGPS = false);
	// ��Las��ͷ�ļ�
	void						ReadLasHeader();
	// �������������ϵ�����б任(ȫ������ϵ���ֲ�����ϵ)
	void						Global2LocalCoord(double gx, double gy, double gz, int & x, int & y, int & z);
	// �������������ϵ�����б任(�ֲ�����ϵ��ȫ������ϵ)
	void						Local2GlobalCoord(int x, int y, int z, double & gx, double & gy, double & gz);
	// ���±߽�
	void						UpdateRange();
};
#endif // WULASLIB_H__WHU_WUTENG_2014_05_16_14_40_03
