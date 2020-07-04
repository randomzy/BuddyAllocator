cmake_minimum_required(VERSION 3.12)

project(BuddyAllocatorPC)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES ${SOURCES}
    
)

set(PUBLIC_INCLUDES ${PUBLIC_INCLUDES}
    ./include
)

set(OPT "-O0")
set(WARN "-Wall")
set(DEFS "")
set(TUNING "") #-fshort-enums -fpack-struct -funsigned-char 
set(FLAGS "${FLAGS} ${DEFS} ${TUNING} ${OPT} ${WARN}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main src/main.cpp ${SOURCES})
target_include_directories(main PUBLIC ${PUBLIC_INCLUDES})