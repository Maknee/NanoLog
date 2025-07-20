cmake_minimum_required(VERSION 3.10)

project(NanoLog VERSION 0.0.1 DESCRIPTION "Nanolog is an extremely performant nanosecond scale logging system for C++ that exposes a simple printf-like API.")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

set(CMAKE_CXX_STANDARD 17)

# Find Python for the preprocessor
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# pre-requisites
find_package(Threads REQUIRED)
find_package(RT REQUIRED)

# Generate GeneratedCode.cc from client.cc using the preprocessor
set(GENERATED_CODE_CC ${CMAKE_CURRENT_BINARY_DIR}/GeneratedCode.cc)
set(CLIENT_CC ${PROJECT_SOURCE_DIR}/runtime/testHelper/client.cc)
set(PARSER_PY ${PROJECT_SOURCE_DIR}/preprocessor/parser.py)

add_custom_command(
    OUTPUT ${GENERATED_CODE_CC}
    COMMAND ${CMAKE_CXX_COMPILER} -DPREPROCESSOR_NANOLOG -E -I${PROJECT_SOURCE_DIR}/runtime ${CLIENT_CC} -o ${CMAKE_CURRENT_BINARY_DIR}/client.cc.i
    COMMAND ${Python3_EXECUTABLE} ${PARSER_PY} --mapOutput=${CMAKE_CURRENT_BINARY_DIR}/client.map ${CMAKE_CURRENT_BINARY_DIR}/client.cc.i
    COMMAND ${Python3_EXECUTABLE} ${PARSER_PY} --combinedOutput=${GENERATED_CODE_CC} ${CMAKE_CURRENT_BINARY_DIR}/client.map
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/client.map ${CMAKE_CURRENT_BINARY_DIR}/client.cc.i ${CMAKE_CURRENT_BINARY_DIR}/client.cc.ii
    DEPENDS ${CLIENT_CC} ${PARSER_PY}
    COMMENT "Generating GeneratedCode.cc from client.cc"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add custom target to ensure GeneratedCode.cc is built
add_custom_target(GeneratedCode DEPENDS ${GENERATED_CODE_CC})

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/runtime/*.h)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/runtime/*.cc)

# Exclude LogDecompressor.cc from the library as it contains main()
list(REMOVE_ITEM SOURCES ${PROJECT_SOURCE_DIR}/runtime/LogDecompressor.cc)

# Add the generated code to sources
list(APPEND SOURCES ${GENERATED_CODE_CC})

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Make sure GeneratedCode is built before the library
add_dependencies(${PROJECT_NAME} GeneratedCode)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_BINARY_DIR}  # For GeneratedCode.h
)

# Add PREPROCESSOR_NANOLOG flag for compatibility with test code
target_compile_definitions(${PROJECT_NAME} PRIVATE PREPROCESSOR_NANOLOG)

# Link required libraries to the main library
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${RT_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

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
    target_compile_definitions(decompressor PRIVATE PREPROCESSOR_NANOLOG)
    target_link_libraries(decompressor ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${RT_LIBRARIES})
    
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
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} 
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(BUILD_DECOMPRESSOR)
    install(TARGETS decompressor DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)