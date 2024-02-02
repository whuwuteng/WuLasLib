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
	double				x;						// x 坐标	
	double				y;						// y 坐标
	double				z;						// z 坐标
	unsigned short		rgb[3];					// RGB颜色
}LasPoint;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_ITEM alread define,be sure it was define as: struct tagLasPoint{ double x,y,z; unsigned short rgb[3]; }. \
               \nWuLasLib.h, 警告:类型 LASPOINT_ITEM 已经定义过,请确保其定义为: struct tagLasPoint{ double x,y,z; unsigned short rgb[3];}")
#endif

#ifndef LASPOINT_CLASS
#define LASPOINT_CLASS
typedef struct tagLasPointClass{
	double				x;						// x 坐标	
	double				y;						// y 坐标
	double				z;						// z 坐标
	unsigned short		rgb[3];					// RGB颜色
	unsigned short		intensity;				// 强度信息
	unsigned char		classification;			// 类别信息
}LasPointClass;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_CLASS alread define,be sure it was define as: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification;}. \
			   \nWuLasLib.h, 警告:类型 LASPOINT_CLASS 已经定义过,请确保其定义为: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification;}")
#endif

#ifndef LASPOINT_FULL
#define LASPOINT_FULL
typedef struct tagLasPointFull{
	double				x;						// x 坐标	
	double				y;						// y 坐标
	double				z;						// z 坐标
	unsigned short		rgb[3];					// RGB颜色
	unsigned short		intensity;				// 强度信息
	unsigned char		classification;			// 类别信息
	unsigned short		point_source_ID;		// 点的ID值
}LasPointFull;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_FULL alread define,be sure it was define as: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned short point_source_ID;}. \
				\nWuLasLib.h, 警告:类型 LASPOINT_FULL 已经定义过,请确保其定义为: struct tagLasPointClass{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned short point_source_ID;}")
#endif

#ifndef LASPOINT_ECHO
#define LASPOINT_ECHO
typedef struct tagLasPointEcho{
	double				x;						// x 坐标	
	double				y;						// y 坐标
	double				z;						// z 坐标
	unsigned short		rgb[3];					// RGB颜色
	unsigned short		intensity;				// 强度信息
	unsigned char		classification;			// 类别信息
	unsigned char       echo;					// 第n次回波
	unsigned char       sumechos;				// 总回波次数
}LasPointEcho;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_ECHO alread define,be sure it was define as: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned char echo; unsigned char sumechos;}. \
			   \nWuLasLib.h, 警告:类型 LASPOINT_ECHO 已经定义过,请确保其定义为: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned short intensity; unsigned char classification; unsigned char echo; unsigned char sumechos;}")
#endif

#ifndef LASPOINT_TIME
#define LASPOINT_TIME
typedef struct tagLasPointTime{
	double				x;						// x 坐标	
	double				y;						// y 坐标
	double				z;						// z 坐标
	unsigned short		rgb[3];					// RGB颜色
	unsigned char       echo;					// 第n次回波
	unsigned char       sumechos;				// 总回波次数
	double              gpstime;                // GPS time
}LasPointTime;
#else
#pragma message("WuLasLib.h, Warning: LASPOINT_TIME alread define,be sure it was define as: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned char echo; unsigned char sumechos; double gpstime;}. \
			   \nWuLasLib.h, 警告:类型 LASPOINT_TIME 已经定义过,请确保其定义为: struct tagLasPointEcho{ double x,y,z; unsigned short rgb[3]; unsigned char echo; unsigned char sumechos; double gpstime;}")
#endif

#ifndef _DPT3D
#define _DPT3D
typedef struct tagDPT3D
{
	double				x;						// x 坐标	
	double				y;						// y 坐标
	double				z;						// z 坐标
}DPT3D;
#else
#pragma  message("WuLasLib.h, Warning: DPT3D alread define,be sure it was define as: struct tagDPT3D{ double x,y,z; }. \
				\nWuLasLib.h, 警告:类型 DPT3D 已经定义过,请确保其定义为: struct tagDPT3D{ double x,y,z; }") 
#endif

// 先实现写的功能
class CWuLasLib
{
public:
	CWuLasLib();
	virtual ~CWuLasLib();

	//! 文件访问模式
	enum OPENFLAGS{ modeRead = 1,						// 以只读的方式打开				
					modeWrite							// 以可写的方式打开
	};

public:
	// 打开操作
	bool						Open(const char * pszLasFile, OPENFLAGS flag = modeRead);
	// 关闭操作
	void						Close();
	// 获得错误信息
	const char *				GetLastError();
/********************************************************************************************************/
	// 写文件的操作,写文件只能用同一种模式
	// 设置偏移量
	void						SetOffset(double xOffset, double yOffset, double zOffset);
	// 设置精度,即点最后的保存到小数点后几位
	void						SetPrecision(double xPrecision, double yPrecision, double zPrecision);
	// 写点云(从上次的地方开始写入)
	bool						WriteLas(LasPoint * pLasPoint, int nPoint);
	// 写点云(从上次的地方开始写入)
	bool						WriteLas(LasPointClass * pLasPoint, int nPoint);
	// 写点云(从上次的地方开始写入)
	bool						WriteLas(LasPointFull * pLasPoint, int nPoint);
	// 写点云(从上次的地方开始写入)
	bool						WriteLas(LasPointEcho * pLasPoint, int nPoint);
	// 写点云(从上次的地方开始写入)
	bool						WriteLas(LasPointTime * pLasPoint, int nPoint);
	// 只写点云的坐标(从上次的地方开始写入)
	bool						WriteLas(DPT3D * pLasPoint, int nPoint);
	// 写点云(中间写入)
	bool						WriteLas(int nStartIndex, LasPoint * pLasPoint, int nListSize);
	// 写点云(中间写入)
	bool						WriteLas(int nStartIndex, LasPointClass * pLasPoint, int nListSize);
	// 写点云(中间写入)
	bool						WriteLas(int nStartIndex, LasPointFull * pLasPoint, int nListSize);
	// 写点云(中间写入)
	bool						WriteLas(int nStartIndex, LasPointEcho * pLasPoint, int nListSize);
	// 写点云(中间写入)
	bool						WriteLas(int nStartIndex, LasPointTime * pLasPoint, int nListSize);
	// 只写点云的坐标(中间写入)
	bool						WriteLas(int nStartIndex, DPT3D * pLasPoint, int nListSize);

	// 读文件的操作
	// 获得点的个数
	int							GetPtNum();
	// 获得范围
	void						GetRange(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax);
	// 获得偏移量
	void						GetOffset(double & xOffset, double & yOffset, double & zOffset);
	// 获得精度,即点最后的保存到小数点后几位
	void						GetPrecision(double & xPrecision, double & yPrecision, double & zPrecision);
	// 读点云(从上次的地方开始读取)
	bool						ReadLas(LasPoint * pLasPoint, int nListSize);
	// 读点云(从上次的地方开始读取)
	bool						ReadLas(LasPointClass * pLasPoint, int nListSize);
	// 读点云(从上次的地方开始读取)
	bool						ReadLas(LasPointFull * pLasPoint, int nListSize);
	// 读点云(从上次的地方开始读取)
	bool						ReadLas(LasPointEcho * pLasPoint, int nListSize);
	// 读点云(从上次的地方开始读取)
	bool						ReadLas(LasPointTime * pLasPoint, int nListSize);
	// 只读取点云的坐标(从上次的地方开始读取)
	bool						ReadLas(DPT3D * pLasPoint, int nListSize);
	// 读点云(中间读取)
	bool						ReadLas(int nStartIndex, LasPoint * pLasPoint, int nListSize);
	// 读点云(中间读取)
	bool						ReadLas(int nStartIndex, LasPointClass * pLasPoint, int nListSize);
	// 读点云(中间读取)
	bool						ReadLas(int nStartIndex, LasPointFull * pLasPoint, int nListSize);
	// 读点云(中间读取)
	bool						ReadLas(int nStartIndex, LasPointEcho * pLasPoint, int nListSize);
	// 读点云(中间读取)
	bool						ReadLas(int nStartIndex, LasPointTime * pLasPoint, int nListSize);
	// 只读取点云的坐标(中间读取)
	bool						ReadLas(int nStartIndex, DPT3D * pLasPoint, int nListSize);
/********************************************************************************************************/

protected:
	// 错误信息
	char						m_szErrorMsg[512];
	// 文件的路径
	char						m_szLasPath[512];
	// 偏移量
	double						m_xOffset;
	double						m_yOffset;
	double						m_zOffset;
	// 缩放系数,写的时候根据精度来
	double						m_xScalefactor;
	double						m_yScalefactor;
	double						m_zScalefactor;
	// 文件标志
	OPENFLAGS					m_flag;
	FILE *						m_pfpFile;
	void *						m_pLasMode;
	// add for the write(only)
	void * 						m_pHeader;
	bool						m_bOpenMode;
	// 点的范围
	double						m_xMax;
	double						m_xMin;
	double						m_yMax;
	double						m_yMin;
	double						m_zMax;
	double						m_zMin;
	// 点的个数
	int							m_nPoint;

protected:
	void						Reset();
	// 写Las的头文件
	bool						WriteLasHeader(bool bColor = true, bool bGPS = false);
	// 读Las的头文件
	void						ReadLasHeader();
	// 根据坐标和缩放系数进行变换(全局坐标系到局部坐标系)
	void						Global2LocalCoord(double gx, double gy, double gz, int & x, int & y, int & z);
	// 根据坐标和缩放系数进行变换(局部坐标系到全局坐标系)
	void						Local2GlobalCoord(int x, int y, int z, double & gx, double & gy, double & gz);
	// 更新边界
	void						UpdateRange();
};
#endif // WULASLIB_H__WHU_WUTENG_2014_05_16_14_40_03
