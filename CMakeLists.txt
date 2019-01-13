cmake_minimum_required(VERSION 3.10)
project(MarsSimulator)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "Current Platform: Linux ")
    include_directories("/usr/local/GDAL/include")
    link_directories("/usr/local/GDAL/lib")
ENDIF()

set(CMAKE_CXX_STANDARD 11)
link_libraries(gdal)
link_libraries(pthread)
find_package(OpenCV REQUIRED)

add_executable(MarsSimulator main.cpp GeoTIFFReader.cpp GeoTIFFReader.h cray.cpp
        cray.h csphere.cpp csphere.h gvector3.cpp gvector3.h intersectresult.h perspectiveCamera.cpp
        perspectiveCamera.h MarsWithCameraModel.cpp MarsWithCameraModel.h directlight.cpp directlight.h
        color.cpp color.h pointlight.cpp pointlight.h)

target_link_libraries(MarsSimulator ${OpenCV_LIBS})
