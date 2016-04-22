cmake_minimum_required(VERSION 2.6)

project(examples CXX)

include_directories("../gsmtrx/core/threading")
include_directories("../gsmtrx/core")
include_directories("../gsmtrx/gsm/radio")
include_directories("../gsmtrx/gsm/L1")
include_directories("../gsmtrx/gsm")
include_directories("../gsmtrx/api")
include_directories("../gsmtrx")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O2" )

add_executable( sample_l1_rx sample_l1_rx.cpp )

target_link_libraries( sample_l1_rx "gsmtrx" )
