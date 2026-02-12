# An adapter IO for Las File Reader and Writer

## Introduction

An implement based on C++ and [LAStools](https://github.com/LAStools/LAStools) read and write [las file](https://www.asprs.org/divisions-committees/lidar-division/laser-las-file-format-exchange-activities). 
In this way, if  [LAStools](https://github.com/LAStools/LAStools) changes a lot, you do not need to change you kernel code.

## Running environment 

### Dependency

For the Las IO, there is **no dependency**, for the Las projection converter, it depends on [**GDAL**](https://gdal.org/).

### MakeLists for Ubuntu 16.04

``` shell
#! /bin/bash

mkdir build
cd build
cmake ..
make
```

## version change log:
### v1.0 based on lastool(v2014)
This version only imports read and write **uncompress** las file.

### v2.0 based on lastool(v2021)
This version only imports read **compress/uncompress** las file and write uncompress las file.

Read the "extended number of point records" for the **LAS1.4** file.

### v3.0 based on lastool(v2024)

This version add the gps time in the structure, an other change is that use the submodule  [LAStools](https://github.com/LAStools/LAStools)  in the code, so that the code can be changed with the LAStools in the later.

There are some files need to be removed in Linux :

```
./rmfile.sh
```

Some code need to be added in "laswriter_las.hpp" and "laswriter_las.cpp".

### v4.0 based on lastool(v2026)

This version can read and write **compress/uncompress** LAS/LAZ file, for the write, if the extension is "laz/LAZ", then the file is compress.

There are some files need to be removed in Linux :

```
./rmfile.sh
```

Some code need to be added in "laswriter_las.hpp", "laswriter_las.cpp", "mydefs.hpp","lasunzipper.cpp","laszipper.cpp", etc.

**I found that new version LASTool can read very large file(54GB), the old one can't.**


## Example

The code is easy to use, here is an example for reader :

```
	// define the path
	char szSrcLas[512] = "";
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
```

There are two example in the [example folder](/example):

+ **LasFileExample** is an example to crop the las file. After compiling the code, there is a test example in the  [data folder](/data):

```
#! /bin/bash

./LasFileExample "../data/Vaihingen_Strip_10-sub.LAS" "../data/Vaihingen_Strip_10-cut.LAS" 497020 5420320 497040 5420420
```

+ **LasFileConvert** is a tool to convert the Las file using [GDAL](https://gdal.org/), the projection file should be in [proj4 format](https://en.wikipedia.org/wiki/PROJ). In the  [data folder](/data), there are two files, i.e [3943_CC43.proj4](/data/3943_CC43.proj4) and [2154_Lambert93.proj4](/data/_Lambert93.proj4), these files can be downloaded from [EPSG](https://epsg.io/3943).

| <img src="/figures/epsg.png" width="700" alt="epsg" /> |
| :----------------------------------------------------------: |
|           *Download proj4 file*           |

There is a testing example in the  [data folder](/data):

```
#! /bin/bash

./LasFileConvert "../data/3943_CC43.las" "../data/3943_CC43.proj4" "../data/2154_Lambert93.proj4" "../data/3943_CC43_convert.las" 
```

After converting, the the result has the same coordinate reference with **2154_Lambert93.las**, then you can visualize them in [CloudCompare](https://www.danielgm.net/cc/):

| <img src="/figures/convert.png" width="800" alt="convert" /> |
| :----------------------------------------------------------: |
|                *Visulization in CloudCompare*                |

In the figure, the **red** points(**3943_CC43_convert.las**) are converted data, this data is from [AI4GEO project](https://www.ai4geo.eu/). And the **white** points(**2154_Lambert93.las**) are from [UMBRA data](https://hal.archives-ouvertes.fr/hal-02370225/document).

## Other information

It is useful to use [LAStools](https://github.com/LAStools/LAStools) to analyze the LAS file, for example :
``` shell
#! /bin/bash
lasinfo -i file.las
```

## MAINTENANCE
If you think you have any problem, contact [Teng Wu]<whuwuteng@gmail.com>
