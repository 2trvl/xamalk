cmake_minimum_required(VERSION 3.9)
project(libs VERSION 1.0.0)

include_directories(include)


set(VT 3.1.528; 3.5.807; 3.51.1057; 
       4.0.1381; 
       5.0.2195; 5.1.2600; 5.2.3790; 
       6.0.6000; 6.0.6001; 6.1.7600; 6.1.8400; 6.2.9200; 6.3.9600; 
       10.0.10240; 10.0.10586, 10.0.14393; 10.0.15063)  ##  my version 10.0.17763

if (${CMAKE_SYSTEM_VERSION} IN_LIST VT AND WIN32)
    add_compile_definitions(VT)
    message(STATUS "VT will be used for your system")

else()
    add_compile_definitions(ANSI)
    message(STATUS "ANSI will be used for your system")

endif(${CMAKE_SYSTEM_VERSION} IN_LIST VT AND WIN32)


message(STATUS "Creating renderer..")

message(STATUS "Installing renderer library..")
add_library(renderer SHARED src/renderer/renderer.c src/renderer/dllmain.c)
install(TARGETS renderer DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/renderer)

message(STATUS "Installing renderer test executable..")
add_executable(renderer-test src/renderer/renderer.c src/renderer/test.c)
install(TARGETS renderer-test DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/renderer)


message(STATUS "Creating events..")

message(STATUS "Installing events library..")
add_library(events SHARED src/events/events.c src/events/dllmain.c)
install(TARGETS events DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/events)

message(STATUS "Installing events test executable..")
add_executable(events-test src/events/events.c src/events/test.c)
install(TARGETS events-test DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/events)
