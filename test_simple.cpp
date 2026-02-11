#include <iostream>
#include "include/memory/Pattern.h"
#include "include/scanner/PatternScanner.h"

// Simple test to verify pattern matching works
int main() {
    std::cout << "=== Testing Game Trainer Components ===" << std::endl;
    
    // Test 1: Pattern creation and matching
    std::cout << "\nTest 1: Pattern Creation" << std::endl;
    try {
        memory::Pattern pattern("8B 05 ?? ?? ?? ??", "Test Pattern");
        std::cout << "✓ Pattern created: " << pattern.toString() << std::endl;
        std::cout << "  Size: " << pattern.size() << " bytes" << std::endl;
        std::cout << "  Name: " << pattern.getName() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Pattern creation failed: " << e.what() << std::endl;
    }
    
    // Test 2: Pattern matching
    std::cout << "\nTest 2: Pattern Matching" << std::endl;
    {
        memory::Pattern pattern("48 8B 05", "Simple Pattern");
        uint8_t testData1[] = {0x48, 0x8B, 0x05, 0x12, 0x34, 0x56, 0x78};
        uint8_t testData2[] = {0x48, 0x8B, 0x06, 0x12, 0x34, 0x56, 0x78};
        
        bool match1 = pattern.matches(testData1);
        bool match2 = pattern.matches(testData2);
        
        std::cout << "✓ Test data 1 matches: " << (match1 ? "YES" : "NO") << std::endl;
        std::cout << "✓ Test data 2 matches: " << (match2 ? "YES" : "NO") << std::endl;
        
        if (match1 && !match2) {
            std::cout << "✓ Pattern matching works correctly!" << std::endl;
        } else {
            std::cout << "✗ Pattern matching test failed" << std::endl;
        }
    }
    
    // Test 3: Wildcard pattern
    std::cout << "\nTest 3: Wildcard Pattern" << std::endl;
    {
        memory::Pattern pattern("48 8B ?? ?? ?? ?? 90", "Wildcard Pattern");
        uint8_t testData[] = {0x48, 0x8B, 0x12, 0x34, 0x56, 0x78, 0x90};
        
        bool match = pattern.matches(testData);
        std::cout << "✓ Wildcard pattern matches: " << (match ? "YES" : "NO") << std::endl;
        
        // Check wildcard positions
        std::cout << "  Wildcard at position 2: " << (pattern.isWildcard(2) ? "YES" : "NO") << std::endl;
        std::cout << "  Wildcard at position 0: " << (pattern.isWildcard(0) ? "YES" : "NO") << std::endl;
    }
    
    // Test 4: PatternResult functionality
    std::cout << "\nTest 4: Pattern Result" << std::endl;
    {
        memory::PatternResult result(0x12345678, "Test Result");
        std::cout << "✓ Result address: 0x" << std::hex << result.address << std::dec << std::endl;
        std::cout << "✓ Result name: " << result.patternName << std::endl;
        
        uintptr_t offsetAddress = result.calculateOffset(0x100);
        std::cout << "✓ Offset calculation: 0x" << std::hex << offsetAddress 
                  << " (base + 0x100)" << std::dec << std::endl;
    }
    
    std::cout << "\n=== All Tests Complete ===" << std::endl;
    std::cout << "\nThe game trainer implements:" << std::endl;
    std::cout << "1. Binary pattern matching with wildcard support" << std::endl;
    std::cout << "2. Memory scanning algorithms (naive and Boyer-Moore)" << std::endl;
    std::cout << "3. Offset calculation from pattern matches" << std::endl;
    std::cout << "4. MinHook integration for function hooking" << std::endl;
    std::cout << "5. Console-based trainer interface" << std::endl;
    std::cout << "6. Mock memory provider for testing" << std::endl;
    
    return 0;
}