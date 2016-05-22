cmake_minimum_required(VERSION 2.8) 
PROJECT (track)
find_package(OpenCV REQUIRED )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}")
include(CheckCXX11Features.cmake)

# GCC specifics
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -std=c++0x")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")
endif()


set( MY_SRC
    source/main.cpp 
    source/operation.cpp
    source/VehicleTracking.cpp
)

set( MY_HEADERS       
     include/VehicleTracking.h
)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )
#link_directories( ${CMAKE_BINARY_DIR}/bin)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable( track ${MY_SRC} ${MY_HEADERS} )
foreach(flag ${CXX11_FEATURE_LIST})
  set_property(TARGET testme
	APPEND PROPERTY COMPILE_DEFINITIONS ${flag}
	)
endforeach()
target_link_libraries( track ${OpenCV_LIBS} )
