# minimal version of CMake expected
cmake_minimum_required(VERSION 3.0)

# project definition
project(RTG-scheduler)
 
# add the include directory for the preprocessor
include_directories(include)

# Add the source files to a variable
set(SOURCES 
source/RTGS-file_handler.c 
source/RTGS-functions.c 
source/RTGS-linked_list.c 
source/RTGS-mode_1.c 
source/RTGS-mode_2.c 
source/RTGS-mode_3.c 
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