# ----------------------------------------------------------------------------
#  Root CMake file for BVRuler
#
#    From the off-tree build directory, invoke:
#      $ cmake <PATH_TO_BVRULER_ROOT>
#
# ----------------------------------------------------------------------------


# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()


include(cmake/BVRulerDependencies.cmake)

if(CMAKE_GENERATOR MATCHES Xcode AND XCODE_VERSION VERSION_GREATER 4.3)
  cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
elseif(CMAKE_SYSTEM_NAME MATCHES WindowsPhone OR CMAKE_SYSTEM_NAME MATCHES WindowsStore)
  cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
  #Required to resolve linker error issues due to incompatibility with CMake v3.0+ policies.
  #CMake fails to find _fseeko() which leads to subsequent linker error.
  #See details here: http://www.cmake.org/Wiki/CMake/Policies
  cmake_policy(VERSION 2.8)
else()
  cmake_minimum_required(VERSION "${MIN_VER_CMAKE}" FATAL_ERROR)
endif()

# Following block can break build in case of cross-compiling
# but CMAKE_CROSSCOMPILING variable will be set only on project(OpenCV) command
# so we will try to detect cross-compiling by the presence of CMAKE_TOOLCHAIN_FILE
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  if(NOT CMAKE_TOOLCHAIN_FILE)
    # it _must_ go before project(OpenCV) in order to work
    if(WIN32)
      set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory")
    else()
      set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation Directory")
    endif()
  else()
    #Android: set output folder to ${CMAKE_BINARY_DIR}
    set(LIBRARY_OUTPUT_PATH_ROOT ${CMAKE_BINARY_DIR} CACHE PATH "root for library output, set this to change where android libs are compiled to" )
    # any cross-compiling
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory")
  endif()
endif()

if(POLICY CMP0026)
  cmake_policy(SET CMP0026 NEW)
endif()

if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

if(POLICY CMP0046)
  cmake_policy(SET CMP0046 NEW)
endif()

if(POLICY CMP0051)
  cmake_policy(SET CMP0051 NEW)
endif()

if(POLICY CMP0056)
  cmake_policy(SET CMP0056 NEW)
endif()

if(POLICY CMP0067)
  cmake_policy(SET CMP0067 NEW)
endif()

# ---[ BVRuler project
project(BVRuler C CXX)

# ---[ Includes
set(BVRuler_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(BVRuler_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
include_directories(${BVRuler_INCLUDE_DIR})
aux_source_directory(${BVRuler_SRC_DIR} BVRuler_SRCS)
add_executable(${PROJECT_NAME} ${BVRuler_SRCS})
target_link_libraries(${PROJECT_NAME} ${BVRuler_LINKER_LIBS})

# ---[ uninstall target
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/Uninstall.cmake
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/Uninstall.cmake)



