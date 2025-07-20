// Example usage of NanoLog in an external project
// Shows how easy it is to use NanoLog with the new CMake configuration

#include "NanoLogCpp17.h"
#include <iostream>

using namespace NanoLog::LogLevels;

int main() {
    // Initialize NanoLog
    NanoLog::setLogFile("./my_app.log");
    NanoLog::setLogLevel(DEBUG);
    
    std::cout << "NanoLog Example Application\n";
    std::cout << "Logging to: ./my_app.log\n";
    
    // Simple logging examples
    NANO_LOG(NOTICE, "Application started successfully");
    NANO_LOG(DEBUG, "Debug message with value: %d", 42);
    NANO_LOG(WARNING, "Warning: processing %s with %d items", "data", 100);
    
    // Log some performance data
    auto start = std::chrono::high_resolution_clock::now();
    
    // Log many messages to test performance
    for (int i = 0; i < 1000; ++i) {
        NANO_LOG(DEBUG, "Performance test message %d", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    NANO_LOG(NOTICE, "Logged 1000 messages in %lu nanoseconds", duration.count());
    
    // Sync logs to disk
    NanoLog::sync();
    
    std::cout << "Logging complete! Check my_app.log\n";
    std::cout << "Use 'decompressor decompress ./my_app.log' to view logs\n";
    
    return 0;
} 