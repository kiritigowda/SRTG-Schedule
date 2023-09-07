# minimum version of CMake expected
cmake_minimum_required(VERSION 3.5)

# project definition
project(SRTG-Scheduler)
set(VERSION "1.2.0")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRTG_PATH /opt/srtg CACHE PATH "SRTG-Scheduler default installation path")
# need to do this to avoid setting the default path to /usr/local
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${SRTG_PATH}/SRTG-Scheduler CACHE PATH "SRTG-Scheduler default installation path" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin CACHE PATH "Directory where SRTG-Scheduler executable is generated" FORCE)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(BoldBlue    "${Esc}[1;34m")
endif()
message("-- ${BoldBlue}SRTG-Scheduler Version -- ${VERSION}${ColourReset}")
message("-- ${BoldBlue}SRTG-Scheduler install path set to -- ${CMAKE_INSTALL_PREFIX}${ColourReset}")
 
# add the include directory for the preprocessor
include_directories(include)

# Add the source files to a variable
set(SOURCES 
    source/profiler.cpp
    source/RTG-scheduler.c
    source/RTGS-AEAP_ALAP_improve.c
    source/RTGS-AEAP_ALT.c
    source/RTGS-AEAP.c
    source/RTGS-ALAP_ALT.c
    source/RTGS-ALAP_improve.c
    source/RTGS-ALAP.c
    source/RTGS-dispatch.c
    source/RTGS-entry.c
    source/RTGS-file_handler.c
    source/RTGS-functions.c
    source/RTGS-helper_functions.c
    source/RTGS-mode_1.c
    source/RTGS-mode_2.c 
    source/RTGS-mode_3.c 
    source/RTGS-mode_4.c 
    source/RTGS-mode_5.c 
    source/RTGS-queue_handler.c
    )

# executable target and the source files
add_executable(SRTG-Scheduler ${SOURCES})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 -std=c++11")
endif()

install(TARGETS SRTG-Scheduler DESTINATION bin)
install(DIRECTORY SRTG-JobCreator DESTINATION .)
install(DIRECTORY SRTG-ResultAnalysis DESTINATION .)

# Bring in default test data
FILE(GLOB RTG_TEST_DATA_TXT "testData/*.txt")
FILE(GLOB RTG_TEST_DATA_CSV "testData/*.csv")
file(COPY ${RTG_TEST_DATA_TXT} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testData/)
file(COPY ${RTG_TEST_DATA_CSV} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testData/)
