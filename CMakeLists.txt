cmake_minimum_required(VERSION 3.10)
project(aclientlibrary VERSION 0.1.1)

# Options
option(DEBUG "Enable debugging" OFF)
option(ADDRESS_SANITIZER "Enable Address Sanitizer" OFF)

set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INSTALL_DOCDIR share/doc/aclientlibrary)
set(CMAKE_C_STANDARD 99)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Find the required libraries
find_package(amemorylibrary REQUIRED)
find_package(themacrolibrary REQUIRED)
find_package(httpparserlibrary REQUIRED)

# Find the libuv package
find_package(libuv REQUIRED)

# Include libuv headers
include_directories(${LIBUV_INCLUDE_DIRS})

# Compiler options
if(ADDRESS_SANITIZER)
    add_compile_options(-fsanitize=address)
endif()

# Source files
set(SOURCE_FILES src/aclient.c)

# Debug library
add_library(aclientlibrary_debug STATIC ${SOURCE_FILES})
target_include_directories(aclientlibrary_debug PUBLIC ${themacrolibrary_INCLUDE_DIRS})
target_include_directories(aclientlibrary_debug PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_definitions(aclientlibrary_debug PUBLIC -D_AML_DEBUG_)
target_compile_options(aclientlibrary_debug PRIVATE -g -O0)
set_target_properties(aclientlibrary_debug PROPERTIES OUTPUT_NAME "aclientlibrary_debug")
target_compile_options(aclientlibrary_debug PRIVATE -g -O0)


# Release library
add_library(aclientlibrary_static STATIC ${SOURCE_FILES})
target_include_directories(aclientlibrary_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(aclientlibrary_static PUBLIC ${themacrolibrary_INCLUDE_DIRS})
target_compile_options(aclientlibrary_static PRIVATE -O3)
set_target_properties(aclientlibrary_static PROPERTIES OUTPUT_NAME "aclientlibrary_static")
target_compile_options(aclientlibrary_static PRIVATE -O3)

# Shared library
add_library(aclientlibrary SHARED ${SOURCE_FILES})
target_include_directories(aclientlibrary PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(aclientlibrary PUBLIC ${themacrolibrary_INCLUDE_DIRS})
target_compile_options(aclientlibrary PRIVATE -O3)
set_target_properties(aclientlibrary PROPERTIES OUTPUT_NAME "aclientlibrary")
target_compile_options(aclientlibrary PRIVATE -O3)

# Link libraries
target_link_libraries(aclientlibrary_debug PUBLIC amemorylibrary httpparserlibrary uv_a)
target_link_libraries(aclientlibrary_static PUBLIC amemorylibrary httpparserlibrary uv_a)
target_link_libraries(aclientlibrary PUBLIC amemorylibrary httpparserlibrary uv)

# Installation of the library
install(TARGETS aclientlibrary aclientlibrary_static aclientlibrary_debug
        EXPORT aclientlibraryTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include)

# Export the targets
install(EXPORT aclientlibraryTargets
        FILE aclientlibraryTargets.cmake
        DESTINATION lib/cmake/aclientlibrary)

# Create and install the config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "aclientlibraryConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_file(aclientlibraryConfig.cmake.in aclientlibraryConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/aclientlibraryConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/aclientlibraryConfigVersion.cmake"
        DESTINATION lib/cmake/aclientlibrary)

# Installation of other files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/a-client-library DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES README.md AUTHORS NEWS.md CHANGELOG.md LICENSE NOTICE DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Uninstall command
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/bin/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
