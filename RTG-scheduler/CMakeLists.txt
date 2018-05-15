# minimal version of CMake expected
cmake_minimum_required(VERSION 3.0)

# project definition
project(RTG-scheduler)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin CACHE PATH "Directory where RTG-scheduler executable is generated" FORCE)

# Get rid of warning
if(POLICY CMP0054)
	cmake_policy(SET CMP0054 OLD)
endif()
 
# add the include directory for the preprocessor
include_directories(include)

# Add the source files to a variable
set(SOURCES 
source/RTGS-file_handler.c 
source/RTGS-functions.c 
source/RTGS-helper_functions.c 
source/RTGS-mode_1.c 
source/RTGS-mode_2.c 
source/RTGS-mode_3.c 
source/RTGS-mode_4.c 
source/RTGS-mode_5.c 
source/RTGS-queue_handler.c 
source/RTGS-ALAP.c
source/RTGS-AEAP.c 
source/RTGS-AEAP_ALAP_improve.c 
source/RTGS-AEAP_ALT.c 
source/RTGS-ALAP_ALT.c 
source/RTGS-ALAP_improve.c 
source/RTG-scheduler.c 
source/RTGS-dispatch.c 
source/RTGS-entry.c
source/profiler.cpp
)

# executable target and the source files
add_executable(RTG-scheduler ${SOURCES})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 -std=c++11")
endif()

# Bring in default test data
FILE(GLOB RTG_TEST_DATA "testData/*.txt")
file(COPY ${RTG_TEST_DATA} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testData/)
