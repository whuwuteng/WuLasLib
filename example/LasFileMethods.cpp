
#include <stdio.h>
#include <string.h>

#include <iostream>

#include "WuLasLib.h"

int main(int argc, char const *argv[])
{
	if (argc != 7){
		std::cout << "LasFileExample srcLasFile tarLasFile minx miny maxx maxy" <<std::endl;
		return -1;
	}
	char szSrcLas[512] = { 0 };
	strcpy(szSrcLas, argv[1]);

	char szTarLas[512] = { 0 };
	strcpy(szTarLas, argv[2]);

	double minx = atof(argv[3]);
	double miny = atof(argv[4]);
	double maxx = atof(argv[5]);
	double maxy = atof(argv[6]);

/*	
	char szSrcLas[512] = "../data/Vaihingen_Strip_10-sub.LAS";
	char szTarLas[512] = "../data/Vaihingen_Strip_10-cut.LAS";
	
	double minx = 497020;
	double miny = 5420320;
	double maxx = 497040;
	double maxy = 5420420;
*/	

	CWuLasLib srcLas;
	srcLas.Open(szSrcLas);
	
	int nPoints = srcLas.GetPtNum();
	
	DPT3D * pLas = new DPT3D[nPoints];
	memset(pLas, 0, sizeof(DPT3D) * nPoints);
	
	srcLas.ReadLas(pLas, nPoints);
	
	double xOffset = 0;
	double yOffset = 0;
	double zOffset = 0;
	srcLas.GetOffset(xOffset, yOffset, zOffset);
	
	double xPrecision = 0;
	double yPrecision = 0;
	double zPrecision = 0;
	srcLas.GetPrecision(xPrecision, yPrecision, zPrecision);
	srcLas.Close();
	
	CWuLasLib tarLas;
	tarLas.Open(szTarLas, CWuLasLib::modeWrite);
	tarLas.SetOffset(xOffset, yOffset, zOffset);
	tarLas.SetPrecision(xPrecision, yPrecision, zPrecision);
	
	for(int i = 0; i < nPoints; ++i){
		if (pLas[i].x > minx && pLas[i].x < maxx && \
			pLas[i].y > miny && pLas[i].y < maxy){
			tarLas.WriteLas(pLas + i, 1);
		}
	}
	tarLas.Close();
	
	delete []pLas;
	return 0;
}
