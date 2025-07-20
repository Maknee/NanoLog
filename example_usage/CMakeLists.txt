# Example CMakeLists.txt for external projects using NanoLog
# This demonstrates how easy it is to use NanoLog in your own projects

cmake_minimum_required(VERSION 3.10)
project(MyApp)

# Find NanoLog - this will automatically set up everything needed
find_package(NanoLog REQUIRED)

# Create your application
add_executable(my_app main.cpp)

# Just link against NanoLog::NanoLog - that's it!
# This automatically provides:
#   - All required include directories
#   - C++17 standard requirement
#   - Threading libraries
#   - RT library (POSIX AIO)
#   - All preprocessor definitions
target_link_libraries(my_app NanoLog::NanoLog)

# Optional: Add your own compile flags
target_compile_options(my_app PRIVATE -Werror=format -O3)

# Alternative usage if you're using FetchContent (for Git submodules, etc.):
#
# include(FetchContent)
# FetchContent_Declare(
#     nanolog
#     GIT_REPOSITORY https://github.com/PlatformLab/NanoLog.git
#     GIT_TAG        main  # or specific version tag
# )
# FetchContent_MakeAvailable(nanolog)
# 
# add_executable(my_app main.cpp)
# target_link_libraries(my_app NanoLog::NanoLog) 