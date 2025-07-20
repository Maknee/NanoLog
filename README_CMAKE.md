# NanoLog CMake Usage Guide

This document explains how to use NanoLog in your C++ projects with modern CMake.

## 🚀 Quick Start

### Method 1: Using find_package() (Recommended)

If NanoLog is installed on your system:

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

# Find NanoLog - this sets up everything automatically
find_package(NanoLog REQUIRED)

add_executable(my_app main.cpp)

# Just link against NanoLog::NanoLog - that's it!
target_link_libraries(my_app NanoLog::NanoLog)
```

### Method 2: Using FetchContent (Git Submodules/Direct)

For embedding NanoLog directly in your project:

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

include(FetchContent)
FetchContent_Declare(
    nanolog
    GIT_REPOSITORY https://github.com/PlatformLab/NanoLog.git
    GIT_TAG        main  # or specific commit/tag
)
FetchContent_MakeAvailable(nanolog)

add_executable(my_app main.cpp)
target_link_libraries(my_app NanoLog::NanoLog)
```

### Method 3: As a Subdirectory

If you have NanoLog as a subdirectory:

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

# Add NanoLog subdirectory
add_subdirectory(path/to/nanolog)

add_executable(my_app main.cpp)
target_link_libraries(my_app NanoLog::NanoLog)
```

## ✨ What You Get Automatically

When you link against `NanoLog::NanoLog`, you automatically get:

- ✅ **Include directories** - All NanoLog headers
- ✅ **C++17 requirement** - Enforced automatically
- ✅ **Threading libraries** - pthread linking
- ✅ **RT library** - POSIX AIO support
- ✅ **Generated code** - Preprocessor output included
- ✅ **Preprocessor definitions** - All necessary flags

## 📝 Simple Usage Example

```cpp
#include "NanoLogCpp17.h"
using namespace NanoLog::LogLevels;

int main() {
    NanoLog::setLogFile("./app.log");
    NanoLog::setLogLevel(DEBUG);
    
    NANO_LOG(NOTICE, "Hello from NanoLog!");
    NANO_LOG(DEBUG, "Value: %d", 42);
    
    NanoLog::sync();
    return 0;
}
```

## 🔧 Building NanoLog

### Standard Build
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### With Clang
```bash
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
make -j$(nproc)
```

### Build Options
```bash
# Control what gets built
cmake -DBUILD_SAMPLE=ON \
      -DBUILD_DECOMPRESSOR=ON \
      -DBUILD_TEST=ON \
      -DBUILD_PERF=OFF \
      ..
```

### Installation
```bash
make install
# Or with custom prefix:
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make install
```

## 📦 What Gets Built

| Target | Description | Size |
|--------|-------------|------|
| `libNanoLog.a` | Main library | ~5MB |
| `decompressor` | Log decompressor tool | ~800KB |
| `sampleApplication` | Example app | ~800KB |

## 🔍 Troubleshooting

### Missing RT Library
If you get linking errors about RT library:
```bash
# Install development packages
sudo apt-get install libc6-dev  # Ubuntu/Debian
```

### C++17 Errors
Make sure your compiler supports C++17:
```bash
gcc --version  # Need GCC 7+ 
clang --version  # Need Clang 5+
```

### Find Package Issues
If `find_package(NanoLog)` fails:
```bash
# Set CMAKE_PREFIX_PATH to installation directory
cmake -DCMAKE_PREFIX_PATH=/path/to/nanolog/install ..

# Or use pkg-config
pkg-config --cflags --libs nanolog
```

## 🎯 Performance

- **Logging latency**: ~16-36 ns/message
- **Compression ratio**: 4.3x
- **C++ standard**: C++17 required
- **Thread-safe**: Yes
- **Platform**: Linux (POSIX AIO required)

## 📚 Advanced Usage

### Custom Compiler Flags
```cmake
target_compile_options(my_app PRIVATE 
    -Werror=format  # Catch format errors early
    -O3             # High performance
    -DNDEBUG        # Release mode
)
```

### Multiple Targets
```cmake
# Use for multiple executables
add_executable(app1 app1.cpp)
add_executable(app2 app2.cpp)

target_link_libraries(app1 NanoLog::NanoLog)
target_link_libraries(app2 NanoLog::NanoLog)
```

### Integration Testing
```cmake
# Test that NanoLog works
enable_testing()
add_test(NAME nanolog_test COMMAND my_app)
```

## 🎉 Migration from GNUmakefile

**Old way** (manual include/link):
```cmake
target_include_directories(my_app PRIVATE /path/to/nanolog/runtime)
target_link_libraries(my_app nanolog pthread rt)
```

**New way** (automatic):
```cmake
target_link_libraries(my_app NanoLog::NanoLog)
```

That's it! Everything else is handled automatically. 