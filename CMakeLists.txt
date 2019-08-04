cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(TicFocuserNG CXX)
include(ExternalProject)

set (TICFOCUSER_VERSION_MAJOR 0)
set (TICFOCUSER_VERSION_MINOR 9)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tic_focuser_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/tic_focuser_config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/indi_ticfocuser-ng.xml.cmake ${CMAKE_CURRENT_BINARY_DIR}/indi_ticfocuser-ng.xml)

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

set(indi_ticfocuser-ng_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/tic_focuser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tic_connection.cpp
   )

add_executable(indi_ticfocuser-ng ${indi_ticfocuser-ng_SRCS})
target_link_libraries(indi_ticfocuser-ng indidriver "${LIBTIC_LDFLAGS}")
install(TARGETS indi_ticfocuser-ng RUNTIME DESTINATION bin )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/indi_ticfocuser-ng.xml DESTINATION ${INDI_DATA_DIR})

