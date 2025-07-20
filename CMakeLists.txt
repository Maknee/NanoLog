cmake_minimum_required(VERSION 3.10)

project(NanoLog VERSION 0.0.1 DESCRIPTION "Nanolog is an extremely performant nanosecond scale logging system for C++ that exposes a simple printf-like API.")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

set(CMAKE_CXX_STANDARD 17)

# Find Python for the preprocessor (needed for compatibility code generation)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# pre-requisites
find_package(Threads REQUIRED)
find_package(RT REQUIRED)

# Generate minimal GeneratedCode.cc from client.cc for compatibility
# This is needed even for C++17 version for error checking and symbol compatibility
set(GENERATED_CODE_CC ${CMAKE_CURRENT_BINARY_DIR}/GeneratedCode.cc)
set(CLIENT_CC ${PROJECT_SOURCE_DIR}/runtime/testHelper/client.cc)
set(PARSER_PY ${PROJECT_SOURCE_DIR}/preprocessor/parser.py)

add_custom_command(
    OUTPUT ${GENERATED_CODE_CC}
    COMMAND ${CMAKE_CXX_COMPILER} -E -I${PROJECT_SOURCE_DIR}/runtime ${CLIENT_CC} -o ${CMAKE_CURRENT_BINARY_DIR}/client.cc.i
    COMMAND ${Python3_EXECUTABLE} ${PARSER_PY} --mapOutput=${CMAKE_CURRENT_BINARY_DIR}/client.map ${CMAKE_CURRENT_BINARY_DIR}/client.cc.i
    COMMAND ${Python3_EXECUTABLE} ${PARSER_PY} --combinedOutput=${GENERATED_CODE_CC} ${CMAKE_CURRENT_BINARY_DIR}/client.map
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/client.map ${CMAKE_CURRENT_BINARY_DIR}/client.cc.i ${CMAKE_CURRENT_BINARY_DIR}/client.cc.ii
    DEPENDS ${CLIENT_CC} ${PARSER_PY}
    COMMENT "Generating compatibility code for C++17 NanoLog"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add custom target to ensure GeneratedCode.cc is built
add_custom_target(GeneratedCode DEPENDS ${GENERATED_CODE_CC})

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/runtime/*.h)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/runtime/*.cc)

# Exclude LogDecompressor.cc from the library as it contains main()
list(REMOVE_ITEM SOURCES ${PROJECT_SOURCE_DIR}/runtime/LogDecompressor.cc)

# Exclude LogTest.cc from the library (test file)
list(REMOVE_ITEM SOURCES ${PROJECT_SOURCE_DIR}/runtime/LogTest.cc)

# Add the generated code to sources for compatibility
list(APPEND SOURCES ${GENERATED_CODE_CC})

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Create an alias target for easier consumption
add_library(NanoLog::NanoLog ALIAS ${PROJECT_NAME})

# Make sure GeneratedCode is built before the library
add_dependencies(${PROJECT_NAME} GeneratedCode)

# Set up include directories - PUBLIC so consumers get them automatically
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/runtime>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  # For GeneratedCode.h
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/runtime  # For internal compilation
)

# Add compile definitions - C++17 is the default API (force disable preprocessor mode)
target_compile_definitions(${PROJECT_NAME} 
    PUBLIC
        # These are needed by consumers
        $<$<COMPILE_FEATURES:cxx_std_17>:>
)

# Set C++ standard requirement for consumers
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Link required libraries - PUBLIC so they propagate to consumers
target_link_libraries(${PROJECT_NAME} 
    PUBLIC
        ${CMAKE_THREAD_LIBS_INIT}
        ${RT_LIBRARIES}
)

# Add compile options for better code safety
target_compile_options(${PROJECT_NAME} PRIVATE -Werror=format)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${HEADERS}"
    EXPORT_NAME NanoLog  # For the alias target
)

# Set pkg-config variables for the enhanced .pc.in file
set(PKGCONF_LIBS_PUB "")
set(PKGCONF_REQ_PRIV "")
set(PKGCONF_LIBS_PRIV "")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

# Build decompressor executable
option(BUILD_DECOMPRESSOR "Build decompressor app" ON)
if(BUILD_DECOMPRESSOR)
    add_executable(decompressor ${PROJECT_SOURCE_DIR}/runtime/LogDecompressor.cc)
    add_dependencies(decompressor GeneratedCode)
    target_include_directories(decompressor PRIVATE 
        ${PROJECT_SOURCE_DIR}/runtime
        ${CMAKE_CURRENT_BINARY_DIR}  # For GeneratedCode.h
    )
    target_compile_definitions(decompressor PRIVATE -UPREPROCESSOR_NANOLOG)
    target_link_libraries(decompressor ${PROJECT_NAME})
    
    # Set compiler flags to match the original makefile
    target_compile_options(decompressor PRIVATE -O3 -g -DNDEBUG)
endif()

option(BUILD_PERF "Build perf app" OFF)
if(BUILD_PERF AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/perf")
    add_subdirectory(perf)
endif()

option(BUILD_SAMPLE "Build sample app" ON)
if(BUILD_SAMPLE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sample")
    add_subdirectory(sample)
endif()

option(BUILD_TEST "Build unit tests" ON)
if(BUILD_TEST AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    enable_testing ()
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)

# Install targets
install(TARGETS ${PROJECT_NAME} 
    EXPORT NanoLogTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} 
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(BUILD_DECOMPRESSOR)
    install(TARGETS decompressor DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

# Generate and install CMake config files for find_package() support
include(CMakePackageConfigHelpers)

# Generate the config file that includes the exports
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NanoLogConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NanoLogConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NanoLog
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Generate the version file for the config file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NanoLogConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install the configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NanoLogConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NanoLogConfigVersion.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindRT.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NanoLog
)

# Install the export set for use with the install-tree
install(EXPORT NanoLogTargets
    FILE NanoLogTargets.cmake
    NAMESPACE NanoLog::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NanoLog
)