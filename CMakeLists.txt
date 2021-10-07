# Copyright 2021 by Martin Moene
#
# https://github.com/martinmoene/active-lite
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompactiveing file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 3.8 FATAL_ERROR )

message( STATUS "CMake version: '${CMAKE_VERSION}'")

# active-lite project and version, updated by script/update-version.py:

project(
    active_lite
    VERSION 0.4.0
#   DESCRIPTION "TODO - for C++98, C++11 and later in a single-file header-only library"
#   HOMEPAGE_URL "https://github.com/martinmoene/active-lite"
    LANGUAGES CXX )

# Package information:

set( unit_name       "active" )
set( package_nspace  "nonstd" )
set( package_name    "${unit_name}-lite" )
set( package_version "${${PROJECT_NAME}_VERSION}" )

message( STATUS "Project '${PROJECT_NAME}', package '${package_name}' version: '${package_version}'")

# Toplevel or subproject:

if ( CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME )
    set( active_IS_TOPLEVEL_PROJECT TRUE )
else()
    set( active_IS_TOPLEVEL_PROJECT FALSE )
endif()

# If toplevel project, enable building and performing of tests, disable building of examples:

option( ACTIVE_LITE_OPT_BUILD_TESTS    "Build and perform active-lite tests" ${active_IS_TOPLEVEL_PROJECT} )
option( ACTIVE_LITE_OPT_BUILD_EXAMPLES "Build active-lite examples" OFF )

# option( active_LITE_OPT_SELECT_STD     "Select std::active"    OFF )
# option( active_LITE_OPT_SELECT_NONSTD  "Select nonstd::active" OFF )

# If requested, build and perform tests, build examples:

if ( ACTIVE_LITE_OPT_BUILD_TESTS )
    enable_testing()
    add_subdirectory( test )
endif()

if ( ACTIVE_LITE_OPT_BUILD_EXAMPLES )
    add_subdirectory( example )
endif()

#
# Interface, installation and packaging
#

# CMake helpers:

include( GNUInstallDirs )
include( CMakePackageConfigHelpers )

# Interface library:

add_library(
    ${package_name} INTERFACE )

add_library(
    ${package_nspace}::${package_name} ALIAS ${package_name} )

target_include_directories(
    ${package_name}
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

# Package configuration:
# Note: package_name and package_target are used in package_config_in

set( package_folder         "${package_name}" )
set( package_target         "${package_name}-targets" )
set( package_config         "${package_name}-config.cmake" )
set( package_config_in      "${package_name}-config.cmake.in" )
set( package_config_version "${package_name}-config-version.cmake" )

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${package_config_in}"
    "${CMAKE_CURRENT_BINARY_DIR}/${package_config}"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${package_folder}"
)

configure_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${package_config_version}.in"
   "${CMAKE_CURRENT_BINARY_DIR}/${package_config_version}" @ONLY
)

# Installation:

install(
    TARGETS      ${package_name}
    EXPORT       ${package_target}
#   INCLUDES DESTINATION "${...}"  # already set via target_include_directories()
)

install(
    EXPORT       ${package_target}
    NAMESPACE    ${package_nspace}::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${package_folder}"
)

install(
    FILES       "${CMAKE_CURRENT_BINARY_DIR}/${package_config}"
                "${CMAKE_CURRENT_BINARY_DIR}/${package_config_version}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${package_folder}"
)

install(
    DIRECTORY   "include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

export(
    EXPORT       ${package_target}
    NAMESPACE    ${package_nspace}::
    FILE         "${CMAKE_CURRENT_BINARY_DIR}/${package_name}-targets.cmake"
)

# end of file
