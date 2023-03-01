### CMake file for sblv_tools
###
###   -  2023: Initial version <druon@lirmm.fr>
###
###
###############################################################################

cmake_minimum_required (VERSION 2.6)

project (sblv_tools)
set (sblv_tools_VERSION_MAJOR 0)
set (sblv_tools_VERSION_MINOR 1)

set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /lib /usr/lib /usr/local/lib )
set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include )
set( CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  "${PROJECT_SOURCE_DIR}/cmake" ) 

set ( INSTALL_BIN_PATH bin )
set ( INSTALL_LIB_PATH lib )
set ( INSTALL_AR_PATH  lib )
set ( INSTALL_HEADERS_PATH include )

# Make relative paths absolute (needed later on)

foreach(p LIB BIN HEADERS AR)
    set(var INSTALL_${p}_PATH)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

### Set include directories 
        
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/include"
	                 "${CMAKE_CURRENT_SOURCE_DIR}/src"
        	         "${CMAKE_CURRENT_BINARY_DIR}"
                     "${CMAKE_INSTALL_PREFIX}/include")

### Main Target and linking

add_executable( "sblv_print" "src/sblv_print.c" )

### Install targets

install( TARGETS "sblv_print"
        RUNTIME DESTINATION ${INSTALL_BIN_PATH}  COMPONENT main
        LIBRARY DESTINATION ${INSTALL_LIB_PATH}  COMPONENT main
        ARCHIVE DESTINATION ${INSTALL_AR_PATH}   COMPONENT main )
   
