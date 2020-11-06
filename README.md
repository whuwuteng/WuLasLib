# An adapter IO for Las File Reader and Writer

An implement based on C++ and [lastool](https://github.com/LAStools/LAStools) read and write [las file](https://www.asprs.org/divisions-committees/lidar-division/laser-las-file-format-exchange-activities). 
In this way, if lastool changes a lot, you do not need to change you kernl code.

## Running environment 
### MakeLists for Ubuntu 16.04
<UL>
<LI> mkdir build
<LI> cd build
<LI> cmake ..
<LI> make
</UL>

## version change log:
### v1.0 based on lastool(v2008)
This version only imports read and write uncompress las file.

### v2.0 based on lastool(v2020)
This version only imports read compress/uncompress las file and write uncompress las file.

## MAINTENANCE
If you think you have any problem, contact Teng Wu <whuwuteng@gmail.com>
