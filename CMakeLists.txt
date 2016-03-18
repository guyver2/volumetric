cmake_minimum_required (VERSION 2.6)
project (Volume)

set ( CMAKE_CXX_FLAGS "-std=c++11 -g -Wno-deprecated-register" )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt4 4.8.6 REQUIRED QtGui QtCore )
SET(QT_USE_QTOPENGL TRUE)
include_directories(/usr/include/qt4 )

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

file(GLOB viewer_SRC
     "volume.cpp"
     "viewer/viewer.cpp"
     "viewer/viewWindow.cpp"
     "viewer/arcball.cpp"
)
file(GLOB drawables_SRC
    "viewer/drawables/*.hpp"
    "viewer/drawables/*.cpp"
)
add_executable(Viewer viewer/main.cpp ${viewer_SRC}  ${drawables_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/viewer)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/viewer/drawables)
target_link_libraries(Viewer Qt4::QtGui Qt4::QtOpenGL Qt4::QtCore GL GLU)


#find_package (Threads) # -> ${CMAKE_THREAD_LIBS_INIT}

find_package( OpenCV REQUIRED )

add_executable(Volume main.cpp ${viewer_SRC}  ${drawables_SRC})
target_link_libraries (Volume ${OpenCV_LIBS} Qt4::QtGui Qt4::QtOpenGL Qt4::QtCore GL GLU)

