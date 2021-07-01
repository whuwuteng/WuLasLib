#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>

#include <gdal_priv.h>
#include <gdal.h>

// geo-reference
#include <ogr_spatialref.h>

#include "WuLasLib.h"

bool ReadProj4(const char * pszFile, std::string & strPrj4)
{
	std::ifstream infile;
	infile.open(pszFile);

	if (! infile.is_open()){
		printf("can not open file %s\n", pszFile);
		return false;
	}
	
	char szLine[512] = { 0 };
	infile.getline(szLine, 512);
	infile.close();

	strPrj4 = std::string(szLine);
	
	return true;
}

int main(int argc, char const *argv[])
{	
	if (argc != 5){
		std::cout << "LasFileConvert LasFile SrcProj4 TarProj4 LasPrj" <<std::endl;
		return -1;
	}

	char szSrcLas[512] = { 0 };
	strcpy(szSrcLas, argv[1]);
	
	char szSrcProj4[512] = { 0 };
	strcpy(szSrcProj4, argv[2]);
	
	char szTarLas[512] = { 0 };
	strcpy(szTarLas, argv[4]);
	
	char szTarProj4[512] = { 0 };
	strcpy(szTarProj4, argv[3]);

/*			
	char szSrcLas[512] = "../data/3943_CC43.las";
	char szSrcProj4[512] = "../data/3943_CC43.proj4";
	char szTarLas[512] = "../data/3943_CC43_convert.las";
	char szTarProj4[512] = "../data/2154_Lambert93.proj4";
*/	
	std::string strSrcProj4;
	ReadProj4(szSrcProj4, strSrcProj4);
	
	std::string strTarProj4;
	ReadProj4(szTarProj4, strTarProj4);
	
	OGRSpatialReference monUtm;
	monUtm.importFromProj4(strSrcProj4.c_str());
	
	OGRSpatialReference monGeo;
	monGeo.importFromProj4(strTarProj4.c_str());

	OGRCoordinateTransformation* pCoordTrans = OGRCreateCoordinateTransformation(&monUtm, &monGeo);
	
	CWuLasLib srcLas;
	srcLas.Open(szSrcLas);
	
	double xOffset = 0;
	double yOffset = 0;
	double zOffset = 0;
	srcLas.GetOffset(xOffset, yOffset, zOffset);
	
	double xPrecision = 0;
	double yPrecision = 0;
	double zPrecision = 0;
	srcLas.GetPrecision(xPrecision, yPrecision, zPrecision);
	
	int nPoints = srcLas.GetPtNum();
	
	LasPoint * pLas = new LasPoint[nPoints];
	memset(pLas, 0, sizeof(LasPoint) * nPoints);
	
	srcLas.ReadLas(pLas, nPoints);
	srcLas.Close();

	for (int i = 0; i < nPoints; ++i){
		double x = pLas[i].x;
		double y = pLas[i].y;
		int reprojected = pCoordTrans->Transform(1, &x, &y);
		
		pLas[i].x = x;
		pLas[i].y = y;
	}

	CWuLasLib tarLas;
	
	tarLas.Open(szTarLas, CWuLasLib::modeWrite);

	// convert the xoffset and yoffset
	int reprojected = pCoordTrans->Transform(1, &xOffset, &yOffset);
	tarLas.SetOffset(xOffset, yOffset, zOffset);
	tarLas.SetPrecision(xPrecision, yPrecision, zPrecision);
	
	tarLas.WriteLas(pLas, nPoints);
	tarLas.Close();
	
	delete []pLas;				pLas = NULL;
	
	OCTDestroyCoordinateTransformation(pCoordTrans);
	return 0;
}