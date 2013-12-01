# CMakeFiles.txt for the Xdisp
#
# Project name
project (xdisp C)

cmake_minimum_required(VERSION 2.6)


SET(XDISP_PACKAGE_VERSION_MAJOR 4)
SET(XDISP_PACKAGE_VERSION_MINOR 3)
SET(XDISP_PACKAGE_VERSION_PATCH 55)

IF(NOT MINC_TOOLKIT_BUILD)
  SET(CPACK_PACKAGE_VERSION_MAJOR ${XDISP_PACKAGE_VERSION_MAJOR})
  SET(CPACK_PACKAGE_VERSION_MINOR ${XDISP_PACKAGE_VERSION_MINOR})
  SET(CPACK_PACKAGE_VERSION_PATCH ${XDISP_PACKAGE_VERSION_PATCH})
  INCLUDE(CPack)
  
  FIND_PACKAGE( LIBMINC )
  FIND_PACKAGE( X11 )
  
#  FIND_PACKAGE( TIFF )
#  FIND_PACKAGE( PNG )
#  FIND_PACKAGE( JPEG )

  INCLUDE( ${LIBMINC_USE_FILE} )
  
ENDIF(NOT MINC_TOOLKIT_BUILD)
  
SET(PACKAGE "xdisp")
SET(PACKAGE_BUGREPORT "a.janke@gmail.com")

SET(PACKAGE_NAME "xdisp")
SET(PACKAGE_VERSION "${XDISP_PACKAGE_VERSION_MAJOR}.${XDISP_PACKAGE_VERSION_MINOR}.${XDISP_PACKAGE_VERSION_PATCH}")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# aliases
SET(VERSION "${PACKAGE_VERSION}")
SET(PROJECT_VERSION "${PACKAGE_VERSION}")

# check for prereqs
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)

CHECK_INCLUDE_FILES(float.h     HAVE_FLOAT_H)
CHECK_INCLUDE_FILES(dlfcn.h     HAVE_FLFCN_H)
CHECK_INCLUDE_FILES(sys/stat.h  HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(sys/ioctl.h HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILES(sys/select.h HAVE_SYS_select_H)
CHECK_INCLUDE_FILES(values.h    HAVE_VALUES_H)
CHECK_INCLUDE_FILES(unistd.h    HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(memory.h    HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(malloc.h    HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(stdlib.h    HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(inttypes.h  HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(string.h    HAVE_STRING_H)
CHECK_INCLUDE_FILES(strings.h   HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(alloca.h    HAVE_ALLOCA_H)
CHECK_INCLUDE_FILES(termios.h   HAVE_TERMIOS_H)

CHECK_FUNCTION_EXISTS(mkstemp  HAVE_MKSTEMP)
CHECK_FUNCTION_EXISTS(tmpnam   HAVE_TMPNAM)
CHECK_FUNCTION_EXISTS(tempnam  HAVE_TEMPNAM)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR)
CHECK_FUNCTION_EXISTS(popen    HAVE_POPEN)
CHECK_FUNCTION_EXISTS(fork     HAVE_WORKING_FORK)
CHECK_FUNCTION_EXISTS(vfork    HAVE_WORKING_VFORK)
CHECK_FUNCTION_EXISTS(fdopen   HAVE_FDOPEN)
CHECK_FUNCTION_EXISTS(strdup   HAVE_STRDUP)
CHECK_FUNCTION_EXISTS(getpwnam HAVE_GETPWNAM) 
CHECK_FUNCTION_EXISTS(select   HAVE_SELECT)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR) 
CHECK_FUNCTION_EXISTS(sysconf  HAVE_SYSCONF)
CHECK_FUNCTION_EXISTS(system   HAVE_SYSTEM)
CHECK_FUNCTION_EXISTS(alloca   HAVE_ALLOCA alloca.h)



#SET(HAVE_PNG  ${PNG_FOUND})
#SET(HAVE_TIFF ${TIFF_FOUND})
#SET(HAVE_JPEG ${JPEG_FOUND})

SET(HAVE_PNG  OFF)
SET(HAVE_TIFF OFF)
SET(HAVE_JPEG OFF)


# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
   "${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake"
   "${CMAKE_CURRENT_BINARY_DIR}/config.h"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h

include_directories( ${MINC_INCLUDE_DIR} )
include_directories( ${TIFF_INCLUDE_DIR} )
include_directories( ${PNG_INCLUDE_DIR} )
include_directories( ${JPEG_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${X11_X11_INCLUDE_PATH} ${X11_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( olgx EZWGL-1.50/include EZWGL-1.50 ${CMAKE_CURRENT_SOURCE_DIR})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
INCLUDE_DIRECTORIES(  ${CMAKE_CURRENT_BINARY_DIR} )


ADD_SUBDIRECTORY(olgx)
ADD_SUBDIRECTORY(EZWGL-1.50)

SET(C_SRC 
  xdisp.h 
  xdisp.c 
  acr_nema.c 
  acr_nema.h 
  element.c 
  element.h 
  ext_roi.c 
  ext_roi.h 
  file_io.c 
  file_io.h 
  group.c 
  group.h 
  message.c message.h 
  bilinear.c buttons.c color.c compress.c defcolor.c evprocs.c exit.c 
  ezwerror.c ezwfs.c ezwhelp.c ezwinfo.c ezwinit.c ezwinput.c ezwplot.c 
  files.c handler.c help.c image.c init.c keypress.c minc.c mip.c 
  motion.c ms_roi.h nneighbour.c olinit.c profile.c read.c reshape.c roi.c 
  roitomin.h roitominc.h scale.c selzoom.c setcicon.c setmicon.c spectral.h 
  tic.c update.c winlev.c xbutton.c xwins.h xwinutil.c 
)

# Compile each source file to a module
add_executable (xdisp  ${C_SRC}  )

# Link all modules to create the executable
IF(NOT MINC_TOOLKIT_BUILD)
  IF (XDISP_BUILD_STATIC)
    SET (LINK_LIBRARIES
        ${LIBMINC_LIBRARIES}
        ${NetCDF_C_LIBRARY}
        ${HDF5_C_LIBRARIES} 
        ${ZLIB_LIBRARIES}
         -lm -pthread)
  ELSE (XDISP_BUILD_STATIC)
    SET (LINK_LIBRARIES   
        ${LIBMINC_LIBRARIES}
        ${NetCDF_C_LIBRARY}
        ${HDF5_C_LIBRARIES}
        ${ZLIB_LIBRARIES})
  ENDIF (XDISP_BUILD_STATIC)
ELSE(NOT MINC_TOOLKIT_BUILD)
  IF(LIBMINC_STATIC_LIBRARIES)
    SET (LINK_LIBRARIES
      ${LIBMINC_STATIC_LIBRARIES}
        m pthread)
  ELSE(LIBMINC_STATIC_LIBRARIES)
    SET (LINK_LIBRARIES
      ${LIBMINC_LIBRARIES}
        m pthread)
  ENDIF( LIBMINC_STATIC_LIBRARIES)    
ENDIF(NOT MINC_TOOLKIT_BUILD)

target_link_libraries(xdisp olgx ezwgl ${LINK_LIBRARIES} ${X11_LIBRARIES}  m )
#${TIFF_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} 

# Generate an install target
install(TARGETS xdisp RUNTIME DESTINATION bin)
