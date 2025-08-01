# Copyright (C) 2021 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause

include(ExternalProject)

if (INTEL_HEXL_PREBUILT)  # Skip download from github
  find_package(HEXL 1.2.6 HINTS ${INTEL_HEXL_HINT_DIR} REQUIRED)
  if (NOT TARGET HEXL::hexl)
    message(FATAL_ERROR, "Intel HEXL not found")
  endif()

  get_target_property(INTEL_HEXL_INCLUDE_DIR HEXL::hexl INTERFACE_INCLUDE_DIRECTORIES)
  message(STATUS "INTEL_HEXL_INCLUDE_DIR: ${INTEL_HEXL_INCLUDE_DIR}")

  get_target_property(INTEL_HEXL_LOCATION HEXL::hexl LOCATION)
  message(STATUS "INTEL_HEXL_LOCATION: ${INTEL_HEXL_LOCATION}")

  include_directories(${INTEL_HEXL_INCLUDE_DIR})

else()
  # ------------------------------------------------------------------------------
  # Download and install Intel HEXL ...
  # ------------------------------------------------------------------------------
  set(INTEL_HEXL_GIT_REPO_URL https://github.com/intel/hexl.git)
  set(INTEL_HEXL_GIT_LABEL v1.2.6)
  set(INTEL_HEXL_DEBUG OFF) # Set to ON/OFF to toggle debugging
  set(INTEL_HEXL_SHARED_LIB ${BUILD_SHARED}) # Set to ON/OFF to toggle shared library build

  if (BUILD_SHARED AND BUILD_STATIC)
    message(FATAL_ERROR "HEXL supports only shared or static build. Set either BUILD_SHARED=OFF or BUILD_STATIC=OFF")
  endif()

  message(STATUS "Downloading Intel HEXL")
  ExternalProject_Add(
    ext_intel_hexl
    PREFIX ext_intel_hexl
    GIT_REPOSITORY ${INTEL_HEXL_GIT_REPO_URL}
    GIT_TAG ${INTEL_HEXL_GIT_LABEL}
    CMAKE_ARGS
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DHEXL_DEBUG=${INTEL_HEXL_DEBUG}
      -DHEXL_BENCHMARK=OFF
      -DHEXL_COVERAGE=OFF
      -DHEXL_DOCS=OFF
      -DHEXL_EXPERIMENTAL=OFF
      -DHEXL_TESTING=OFF
      -DHEXL_SHARED_LIB=${INTEL_HEXL_SHARED_LIB}
      -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      UPDATE_COMMAND ""
      EXCLUDE_FROM_ALL TRUE)

  if (INTEL_HEXL_SHARED_LIB)
    add_library(HEXL::hexl SHARED IMPORTED GLOBAL)
    if (INTEL_HEXL_DEBUG)
      set_property(TARGET HEXL::hexl PROPERTY
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libhexl_debug${CMAKE_SHARED_LIBRARY_SUFFIX})
    else()
      set_property(TARGET HEXL::hexl PROPERTY
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libhexl${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
  else()
    add_library(HEXL::hexl STATIC IMPORTED GLOBAL)
    if (INTEL_HEXL_DEBUG)
      set_property(TARGET HEXL::hexl PROPERTY
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libhexl_debug.a)
    else()
      set_property(TARGET HEXL::hexl PROPERTY
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libhexl.a)
    endif()
  endif()

  include_directories(${CMAKE_INSTALL_PREFIX}/include)
  add_dependencies(HEXL::hexl ext_intel_hexl)

endif()

find_package(Threads REQUIRED)
target_link_libraries(HEXL::hexl INTERFACE Threads::Threads)
