# minimal version of CMake expected
cmake_minimum_required(VERSION 3.0)

# project definition
project(RTG-scheduler)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin CACHE PATH "Directory where .exe and .dll files are dumped." FORCE)
 
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
source/RTGS_ALAP.c
source/RTGS-AEAP.c 
source/RTGS-AEAP_ALAP_improve.c 
source/RTGS-AEAP_ALT.c 
source/RTGS-ALAP_ALT.c 
source/RTGS-ALAP_improve.c 
source/RTG-scheduler.c 
source/RTGS-dispatch.c 
source/RTGS-entry.c
)

# executable target and the source files
add_executable(RTG-scheduler ${SOURCES})

# Bring in default test data
FILE(GLOB RTG_TEST_DATA "testData/*.txt")
file(COPY ${RTG_TEST_DATA} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testData/)