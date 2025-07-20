cmake_minimum_required(VERSION 3.10)
project(NanoLogHeaderDemo)

# Find NanoLog - this automatically sets up all include paths
find_package(NanoLog REQUIRED)

# Create the demo executable
add_executable(header_demo header_example.cpp)

# Link against NanoLog::NanoLog - this automatically provides:
#   - All include directories (headers are automatically available)
#   - C++17 requirement
#   - Threading libraries
#   - RT library
#   - All preprocessor definitions
target_link_libraries(header_demo NanoLog::NanoLog)

# Optional: Add warning for format errors
target_compile_options(header_demo PRIVATE -Werror=format) 