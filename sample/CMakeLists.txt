# Sample application CMakeLists.txt
# This builds the sampleApplication equivalent to the GNUmakefile
# 
# MODERN USAGE (recommended):
# Just link against NanoLog::NanoLog - everything else is automatic!

# Add the sample application executable
add_executable(sampleApplication main.cc)

# Easy modern approach - just link against NanoLog::NanoLog
# This automatically provides:
#   - Include directories (runtime headers + GeneratedCode.h)
#   - C++17 requirement
#   - Required system libraries (threads, rt)
#   - Preprocessor definitions
target_link_libraries(sampleApplication NanoLog::NanoLog)

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

# Install the sample application (optional)
install(TARGETS sampleApplication DESTINATION ${CMAKE_INSTALL_BINDIR}) 