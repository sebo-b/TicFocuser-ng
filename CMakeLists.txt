cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(astromate CXX C)
include(ExternalProject)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

find_package(INDI REQUIRED)
find_package(PkgConfig)

pkg_check_modules(LIBTIC REQUIRED libpololu-tic-1)
string (REPLACE ";" " " LIBTIC_CFLAGS "${LIBTIC_CFLAGS}")
string (REPLACE ";" " " LIBTIC_LDFLAGS "${LIBTIC_LDFLAGS}")

#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBTIC_CFLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_CFLAGS} ${LIBTIC_CFLAGS}")


#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

include_directories( ${CMAKE_CURRENT_BINARY_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${INDI_INCLUDE_DIR})

set(indi_ticfocus_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/tic_focus.cpp
   )

add_executable(indi_ticfocus ${indi_ticfocus_SRCS})
target_link_libraries(indi_ticfocus indidriver "${LIBTIC_LDFLAGS}")
install(TARGETS indi_ticfocus RUNTIME DESTINATION bin )
install(FILES indi_ticfocus.xml DESTINATION ${INDI_DATA_DIR})

