# Sample application CMakeLists.txt
# This builds the sampleApplication equivalent to the GNUmakefile

# Add the sample application executable
add_executable(sampleApplication main.cc)

# Make sure GeneratedCode is built before the sample application
add_dependencies(sampleApplication GeneratedCode)

# Include the NanoLog runtime directory and build directory for generated code
target_include_directories(sampleApplication PRIVATE 
    ${PROJECT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_BINARY_DIR}/..  # For GeneratedCode.h in parent build dir
)

# Set compiler flags to match the original makefile
# -Werror=format should _always_ be enabled to catch formatting errors
# early during compilation rather than at decompression.
# -DNDEBUG and -O3 are for high performance
target_compile_options(sampleApplication PRIVATE 
    -Werror=format 
    -DNDEBUG 
    -O3 
    -g
)

# Link against NanoLog library, pthreads, and POSIX aio
target_link_libraries(sampleApplication 
    ${PROJECT_NAME} 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${RT_LIBRARIES}
)

# Install the sample application (optional)
install(TARGETS sampleApplication DESTINATION ${CMAKE_INSTALL_BINDIR}) 