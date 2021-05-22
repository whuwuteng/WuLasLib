# An adapter IO for Las File Reader and Writer

An implement based on C++ and [LAStools](https://github.com/LAStools/LAStools) read and write [las file](https://www.asprs.org/divisions-committees/lidar-division/laser-las-file-format-exchange-activities). 
In this way, if lastool changes a lot, you do not need to change you kernl code.

## Running environment 
### MakeLists for Ubuntu 16.04

``` shell
#! /bin/bash

mkdir build
cd build
cmake ..
make
```

## version change log:
### v1.0 based on lastool(v2008)
This version only imports read and write uncompress las file.

### v2.0 based on lastool(v2021)
This version only imports read compress/uncompress las file and write uncompress las file.

Read the "extended number of point records" for the LAS1.4 file.

## Other information

It is useful to use [LAStools](https://github.com/LAStools/LAStools) to analyze the LAS file, for example :
``` shell
#! /bin/bash
lasinfo -i file.las
```

## MAINTENANCE
If you think you have any problem, contact [Teng Wu ]<whuwuteng@gmail.com>
