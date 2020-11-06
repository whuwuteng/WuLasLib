project(MicmacTools)

# debug is too slow fo CGAL
#SET(CMAKE_BUILD_TYPE "Release")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g -ggdb ")
#SET(CMAKE_CXX_FLAGS_RELEASE "${ENV{CXXFLAGS} -O3 -Wall")

#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")


aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/lib libWuLasLib_src)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/lib/lastool lastool_src)

include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/lastool)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

add_library(WuLasLib SHARED ${lastool_src} ${libWuLasLib_src})
add_executable(LasFileExample ${CMAKE_CURRENT_LIST_DIR}/example/LasFileMethods.cpp)

target_link_libraries(LasFileExample WuLasLib)

