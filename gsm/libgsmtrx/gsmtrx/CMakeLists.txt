cmake_minimum_required(VERSION 2.6)

project(gsmtrx CXX)

include_directories("core/threading")
include_directories("core")
include_directories("gsm/radio")
include_directories("gsm/L1")
include_directories("gsm")
include_directories("api")
include_directories(".")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O2" )

file( GLOB MAIN_SOURCES core/threading/*.cpp core/*.cpp gsm/radio/*.cpp gsm/L1/*.cpp api/*.cpp gsm/*.cpp *.cpp )

add_library( gsmtrx SHARED ${MAIN_SOURCES} )

target_link_libraries( gsmtrx "pthread" )
target_link_libraries( gsmtrx "bladeRF" )
