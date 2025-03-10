# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.17)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget hiredis::hiredis)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target hiredis::hiredis
add_library(hiredis::hiredis SHARED IMPORTED)

set_target_properties(hiredis::hiredis PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "C:/Users/Administrator/Desktop/hiredis-master"
)

# Import target "hiredis::hiredis" for configuration "Debug"
set_property(TARGET hiredis::hiredis APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hiredis::hiredis PROPERTIES
  IMPORTED_IMPLIB_DEBUG "C:/Users/Administrator/Desktop/hiredis-master/Debug/hiredis.lib"
  IMPORTED_LOCATION_DEBUG "C:/Users/Administrator/Desktop/hiredis-master/Debug/hiredis.dll"
  )

# Import target "hiredis::hiredis" for configuration "Release"
set_property(TARGET hiredis::hiredis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(hiredis::hiredis PROPERTIES
  IMPORTED_IMPLIB_RELEASE "C:/Users/Administrator/Desktop/hiredis-master/Release/hiredis.lib"
  IMPORTED_LOCATION_RELEASE "C:/Users/Administrator/Desktop/hiredis-master/Release/hiredis.dll"
  )

# Import target "hiredis::hiredis" for configuration "MinSizeRel"
set_property(TARGET hiredis::hiredis APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(hiredis::hiredis PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "C:/Users/Administrator/Desktop/hiredis-master/MinSizeRel/hiredis.lib"
  IMPORTED_LOCATION_MINSIZEREL "C:/Users/Administrator/Desktop/hiredis-master/MinSizeRel/hiredis.dll"
  )

# Import target "hiredis::hiredis" for configuration "RelWithDebInfo"
set_property(TARGET hiredis::hiredis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(hiredis::hiredis PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "C:/Users/Administrator/Desktop/hiredis-master/RelWithDebInfo/hiredis.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "C:/Users/Administrator/Desktop/hiredis-master/RelWithDebInfo/hiredis.dll"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
