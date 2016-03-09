cmake_minimum_required (VERSION 2.6)
project (Volume)

set ( CMAKE_CXX_FLAGS "-std=c++11 -g" )

#find_package (Threads) # -> ${CMAKE_THREAD_LIBS_INIT}

find_package( OpenCV REQUIRED )

add_executable(Volume main.cpp Volume.cpp)
target_link_libraries (Volume ${OpenCV_LIBS})

