cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(TicFocuser CXX)
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
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBTIC_CFLAGS}")

include_directories( ${CMAKE_CURRENT_BINARY_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${INDI_INCLUDE_DIR})

set(indi_ticfocuser_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/tic_focuser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tic_connection.cpp
   )

add_executable(indi_ticfocuser ${indi_ticfocuser_SRCS})
target_link_libraries(indi_ticfocuser indidriver "${LIBTIC_LDFLAGS}")
install(TARGETS indi_ticfocuser RUNTIME DESTINATION bin )
install(FILES indi_ticfocuser.xml DESTINATION ${INDI_DATA_DIR})

