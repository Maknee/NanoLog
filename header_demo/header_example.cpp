// Complete NanoLog Header Usage Examples
// This shows all the available headers and how to include them

// ==========================================
// METHOD 1: C++17 NanoLog (RECOMMENDED)
// ==========================================
#include "NanoLogCpp17.h"  // Main C++17 interface - includes everything you need

// ==========================================
// METHOD 2: Preprocessor NanoLog (Advanced)
// ==========================================
// #include "NanoLog.h"     // For preprocessor-based NanoLog (advanced usage)

// ==========================================
// METHOD 3: Individual Headers (Expert)
// ==========================================
// If you need specific functionality only:
// #include "RuntimeLogger.h"  // Core runtime logger
// #include "Log.h"           // Low-level logging
// #include "Cycles.h"        // CPU cycle counting
// #include "Packer.h"        // Data packing utilities
// #include "Config.h"        // Configuration constants

#include <iostream>
#include <chrono>

// Import log levels for convenience
using namespace NanoLog::LogLevels;

int main() {
    std::cout << "=== NanoLog Header Usage Examples ===\n\n";
    
    // ==========================================
    // BASIC SETUP
    // ==========================================
    std::cout << "1. Setting up NanoLog...\n";
    NanoLog::setLogFile("./example.log");
    NanoLog::setLogLevel(DEBUG);
    
    // ==========================================
    // BASIC LOGGING
    // ==========================================
    std::cout << "2. Basic logging examples...\n";
    
    NANO_LOG(DEBUG, "Debug message");
    NANO_LOG(NOTICE, "Notice message");  
    NANO_LOG(WARNING, "Warning message");
    NANO_LOG(ERROR, "Error message");
    
    // ==========================================
    // FORMATTED LOGGING
    // ==========================================
    std::cout << "3. Formatted logging examples...\n";
    
    int value = 42;
    double pi = 3.14159;
    const char* str = "Hello";
    
    NANO_LOG(NOTICE, "Integer: %d", value);
    NANO_LOG(NOTICE, "Double: %.2f", pi);
    NANO_LOG(NOTICE, "String: %s", str);
    NANO_LOG(NOTICE, "Multiple: %s has %d items, avg %.1f", str, value, pi);
    
    // ==========================================
    // PERFORMANCE TESTING
    // ==========================================
    std::cout << "4. Performance test...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        NANO_LOG(DEBUG, "Performance test message %d", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    NANO_LOG(NOTICE, "Logged 1000 messages in %ld nanoseconds", duration.count());
    
    // ==========================================
    // ADVANCED FEATURES
    // ==========================================
    std::cout << "5. Advanced features...\n";
    
    // Pre-allocate thread-local storage for better performance
    NanoLog::preallocate();
    
    // Check current log level
    auto currentLevel = NanoLog::getLogLevel();
    NANO_LOG(NOTICE, "Current log level: %d", static_cast<int>(currentLevel));
    
    // Get performance statistics
    std::string stats = NanoLog::getStats();
    std::cout << "NanoLog Statistics:\n" << stats << "\n";
    
    // Flush logs to disk
    NanoLog::sync();
    
    std::cout << "\n=== Complete! Check 'example.log' ===\n";
    std::cout << "Use: decompressor decompress ./example.log\n";
    
    return 0;
} 