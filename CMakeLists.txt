project(LasTools)

SET(CMAKE_BUILD_TYPE "Release")

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

FIND_PACKAGE(GDAL REQUIRED)
include_directories(${GDAL_INCLUDE_DIR})

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/lib libWuLasLib_src)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/LAStools/LASlib/src laslib_src)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/LAStools/LASzip/src laszip_src)

include_directories(${CMAKE_CURRENT_LIST_DIR}/LAStools/LASlib/inc)
include_directories(${CMAKE_CURRENT_LIST_DIR}/LAStools/LASlib/src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/LAStools/LASzip/src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/LAStools/LASzip/include/laszip)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

add_library(WuLasLib SHARED ${laslib_src} ${laszip_src} ${libWuLasLib_src})
add_executable(LasFileExample ${CMAKE_CURRENT_LIST_DIR}/example/LasFileMethods.cpp)
add_executable(LasFileConvert ${CMAKE_CURRENT_LIST_DIR}/example/LasConvert.cpp)

target_link_libraries(LasFileExample WuLasLib)
target_link_libraries(LasFileConvert WuLasLib)
target_link_libraries(LasFileConvert ${GDAL_LIBRARY})
