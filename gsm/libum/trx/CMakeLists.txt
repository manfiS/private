cmake_minimum_required(VERSION 2.6)

project(trxgsm CXX)

set( PROJECT "gsmtrx" )

include_directories("src/core/threading")
include_directories("src/core")
include_directories("src/radio")
include_directories("src/gsm")
include_directories("src")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O2" )

#set( CMAKE_CPP_FLAGS "-Wall -Wno-overloaded-virtual" )

file( GLOB MAIN_SOURCES src/core/threading/*.cpp src/core/*.cpp src/radio/*.cpp src/gsm/*.cpp src/*.cpp )

add_executable( ${PROJECT} ${MAIN_SOURCES} )

#set( CMAKE_COMPILE_FLAGS ${CMAKE_CPP_FLAGS})

target_link_libraries( ${PROJECT} "pthread" )
target_link_libraries( ${PROJECT} "bladeRF" )
