cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(TicFocuserNG CXX)
#include(ExternalProject)

set (TICFOCUSER_VERSION_MAJOR 0)
set (TICFOCUSER_VERSION_MINOR 9)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/TicFocuser_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/TicFocuser_config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/indi_ticfocuser-ng.xml.in ${CMAKE_CURRENT_BINARY_DIR}/indi_ticfocuser-ng.xml)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

find_package(INDI REQUIRED)
#find_package(PkgConfig)

find_package(LIBUSB_1)
find_package(LIBBLUETOOTH)

pkg_check_modules(LIBTIC-1 libpololu-tic-1)
#string (REPLACE ";" " " LIBTIC-1_CFLAGS "${LIBTIC-1_CFLAGS}")
#string (REPLACE ";" " " LIBTIC-1_LDFLAGS "${LIBTIC-1_LDFLAGS}")

#pkg_check_modules(LIBBLUETOOTH libbluetooth)

#message("${LIBUSB_1_INCLUDE_DIRS}")

#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBTIC-1_CFLAGS}")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBTIC-1_CFLAGS}")

include_directories( ${CMAKE_CURRENT_BINARY_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR})

include_directories( 
	{CMAKE_CURRENT_BINARY_DIR}
	${INDI_INCLUDE_DIR} 
	${LIBTIC-1_INCLUDE_DIR} 
	${LIBUSB_1_INCLUDE_DIRS} )

set(indi_ticfocuser-ng_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/TicFocuser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tic/TicBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tic/TicUsb.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/connection/UsbConnectionBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/connection/PololuUsbConnection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/connection/LibUsbConnection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/connection/mediators/PololuUsbConnectionMediator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/connection/mediators/TicBaseConnectionMediator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/connection/mediators/LibUsbConnectionMediator.cpp

    )

add_executable(indi_ticfocuser-ng ${indi_ticfocuser-ng_SRCS})

target_compile_options(indi_ticfocuser-ng PUBLIC
	${LIBTIC-1_CFLAGS}
)

target_link_libraries( indi_ticfocuser-ng 
	indidriver 
	"${LIBTIC-1_LIBRARIES}"
	"${LIBUSB_1_LIBRARIES}"
	"${LIBBLUETOOTH_LIBRARIES}"
	)

#target_include_directories( indi_ticfocuser-ng PUBLIC
#	${LIBTIC-1_CFLAGS}
#	)

install(TARGETS indi_ticfocuser-ng RUNTIME DESTINATION bin )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/indi_ticfocuser-ng.xml DESTINATION ${INDI_DATA_DIR})

