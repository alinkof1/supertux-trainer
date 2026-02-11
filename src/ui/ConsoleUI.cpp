#include "ui/ConsoleUI.h"
#include "scanner/PatternScanner.h"
#include "memory/Pattern.h"
#include "hooks/MinHookWrapper.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <memory>

namespace ui {

ConsoleUI::ConsoleUI(std::unique_ptr<scanner::PatternScanner> scanner)
    : m_scanner(std::move(scanner)), m_running(true) {
    
    // Initialize MinHook
    hooks::MHStatus status = hooks::MinHookWrapper::initialize();
    if (status != hooks::MHStatus::MH_OK) {
        std::cerr << "Failed to initialize MinHook: " 
                  << hooks::MinHookWrapper::getLastError() << std::endl;
    }
}

ConsoleUI::~ConsoleUI() {
    // Clean up hooks
    for (auto& hook : m_hooks) {
        hook->remove();
    }
    
    hooks::MinHookWrapper::uninitialize();
}

void ConsoleUI::run() {
    std::cout << "=== Game Trainer Console ===" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    
    while (m_running) {
        std::cout << "\n> ";
        std::string command;
        std::getline(std::cin, command);
        
        processCommand(command);
    }
}

void ConsoleUI::processCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == "help" || cmd == "?") {
        showHelp();
    } else if (cmd == "exit" || cmd == "quit") {
        m_running = false;
        std::cout << "Exiting..." << std::endl;
    } else if (cmd == "scan") {
        processScanCommand(iss);
    } else if (cmd == "patterns") {
        showPatterns();
    } else if (cmd == "hook") {
        processHookCommand(iss);
    } else if (cmd == "hooks") {
        showHooks();
    } else if (cmd == "memory") {
        processMemoryCommand(iss);
    } else if (cmd == "test") {
        runTests();
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
    }
}

void ConsoleUI::showHelp() {
    std::cout << "\nAvailable commands:" << std::endl;
    std::cout << "  help, ?          - Show this help" << std::endl;
    std::cout << "  exit, quit       - Exit the trainer" << std::endl;
    std::cout << "  scan <pattern>   - Scan for a pattern" << std::endl;
    std::cout << "  patterns         - Show available patterns" << std::endl;
    std::cout << "  hook <addr> <fn> - Create a hook at address" << std::endl;
    std::cout << "  hooks            - Show active hooks" << std::endl;
    std::cout << "  memory <addr>    - Read memory at address" << std::endl;
    std::cout << "  test             - Run demonstration tests" << std::endl;
}

void ConsoleUI::processScanCommand(std::istringstream& iss) {
    std::string patternStr;
    std::getline(iss, patternStr);
    
    // Remove leading/trailing whitespace
    patternStr.erase(0, patternStr.find_first_not_of(" \t"));
    patternStr.erase(patternStr.find_last_not_of(" \t") + 1);
    
    if (patternStr.empty()) {
        std::cout << "Usage: scan <pattern>" << std::endl;
        std::cout << "Example: scan \"8B 05 ?? ?? ?? ??" << std::endl;
        return;
    }
    
    try {
        memory::Pattern pattern(patternStr, "User Pattern");
        memory::PatternResult result;
        
        std::cout << "Scanning for pattern: " << pattern.toString() << std::endl;
        
        if (m_scanner->scanEntireProcess(pattern, result)) {
            std::cout << "Pattern found at: 0x" << std::hex << result.address << std::dec << std::endl;
            std::cout << "Matched bytes: ";
            for (uint8_t byte : result.matchedBytes) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                         << static_cast<int>(byte) << " ";
            }
            std::cout << std::dec << std::endl;
            
            // Store the result
            m_scanResults.push_back(result);
        } else {
            std::cout << "Pattern not found" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void ConsoleUI::showPatterns() {
    std::cout << "\nAvailable patterns for SuperTux:" << std::endl;
    std::cout << "1. Health access: \"8B 05 ?? ?? ?? ??\" - Finds health variable access" << std::endl;
    std::cout << "2. Coin update: \"01 1D ?? ?? ?? ??\" - Finds coin count update" << std::endl;
    std::cout << "3. Function prologue: \"55 8B EC\" - Finds function beginnings for hooking" << std::endl;
    std::cout << "\nRecent scan results:" << std::endl;
    
    if (m_scanResults.empty()) {
        std::cout << "No scan results yet" << std::endl;
    } else {
        for (size_t i = 0; i < m_scanResults.size(); ++i) {
            const auto& result = m_scanResults[i];
            std::cout << i + 1 << ". " << result.patternName 
                     << " at 0x" << std::hex << result.address << std::dec << std::endl;
        }
    }
}

void ConsoleUI::processHookCommand(std::istringstream& iss) {
    std::string addrStr, hookName;
    iss >> addrStr >> hookName;
    
    if (addrStr.empty()) {
        std::cout << "Usage: hook <address> <hook_name>" << std::endl;
        std::cout << "Example: hook 0x12345678 health_hook" << std::endl;
        return;
    }
    
    try {
        uintptr_t address = std::stoul(addrStr, nullptr, 16);
        
        // Create a mock hook function address
        uintptr_t hookFunction = address + 0x1000; // Mock hook function
        
        auto hook = std::make_unique<hooks::FunctionHook>(
            hookName.empty() ? "unnamed_hook" : hookName,
            address, hookFunction, hooks::HookType::HOOK_JMP);
        
        if (hook->install()) {
            if (hook->enable()) {
                std::cout << "Hook created and enabled at 0x" << std::hex << address << std::dec << std::endl;
                m_hooks.push_back(std::move(hook));
            } else {
                std::cout << "Hook created but failed to enable" << std::endl;
            }
        } else {
            std::cout << "Failed to create hook: " << hooks::MinHookWrapper::getLastError() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void ConsoleUI::showHooks() {
    std::cout << "\nActive hooks:" << std::endl;
    
    if (m_hooks.empty()) {
        std::cout << "No active hooks" << std::endl;
    } else {
        for (size_t i = 0; i < m_hooks.size(); ++i) {
            const auto& hook = m_hooks[i];
            std::cout << i + 1 << ". " << (hook->isEnabled() ? "[ENABLED] " : "[DISABLED] ")
                     << "Hook at 0x" << std::hex << hook->getOriginal() << std::dec << std::endl;
        }
    }
}

void ConsoleUI::processMemoryCommand(std::istringstream& iss) {
    std::string addrStr;
    iss >> addrStr;
    
    if (addrStr.empty()) {
        std::cout << "Usage: memory <address>" << std::endl;
        std::cout << "Example: memory 0x500000" << std::endl;
        return;
    }
    
    try {
        uintptr_t address = std::stoul(addrStr, nullptr, 16);
        
        // Read 16 bytes
        uint8_t buffer[16];
        auto* provider = m_scanner->getMemoryProvider();
        
        if (provider && provider->readMemory(address, buffer, sizeof(buffer))) {
            std::cout << "Memory at 0x" << std::hex << address << ":" << std::dec << std::endl;
            std::cout << "Hex: ";
            for (int i = 0; i < 16; ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                         << static_cast<int>(buffer[i]) << " ";
            }
            std::cout << std::dec << std::endl;
            
            std::cout << "ASCII: ";
            for (int i = 0; i < 16; ++i) {
                char c = buffer[i];
                std::cout << (c >= 32 && c < 127 ? c : '.');
            }
            std::cout << std::endl;
        } else {
            std::cout << "Failed to read memory at 0x" << std::hex << address << std::dec << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void ConsoleUI::runTests() {
    std::cout << "\n=== Running Demonstration Tests ===" << std::endl;
    
    // Test 1: Pattern scanning
    std::cout << "\nTest 1: Pattern Scanning" << std::endl;
    memory::Pattern healthPattern("8B 05 ?? ?? ?? ??", "Health Access");
    memory::PatternResult healthResult;
    
    if (m_scanner->scanEntireProcess(healthPattern, healthResult)) {
        std::cout << "✓ Found health pattern at 0x" << std::hex << healthResult.address << std::dec << std::endl;
    } else {
        std::cout << "✗ Health pattern not found (expected in mock data)" << std::endl;
    }
    
    // Test 2: Hook creation
    std::cout << "\nTest 2: Hook Creation" << std::endl;
    if (hooks::MinHookWrapper::isInitialized()) {
        std::cout << "✓ MinHook initialized" << std::endl;
        
        // Create a test hook
        uintptr_t testAddress = 0x400000;
        uintptr_t hookFunction = 0x401000;
        uintptr_t originalFunction = 0;
        
        hooks::MHStatus status = hooks::MinHookWrapper::createHook(
            testAddress, hookFunction, &originalFunction);
        
        if (status == hooks::MHStatus::MH_OK) {
            std::cout << "✓ Hook created successfully" << std::endl;
            
            // Enable the hook
            status = hooks::MinHookWrapper::enableHook(reinterpret_cast<void*>(testAddress));
            if (status == hooks::MHStatus::MH_OK) {
                std::cout << "✓ Hook enabled successfully" << std::endl;
                
                // Clean up
                hooks::MinHookWrapper::removeHook(reinterpret_cast<void*>(testAddress));
                std::cout << "✓ Hook cleaned up" << std::endl;
            }
        } else {
            std::cout << "✗ Hook creation failed: " << hooks::MinHookWrapper::getLastError() << std::endl;
        }
    } else {
        std::cout << "✗ MinHook not initialized" << std::endl;
    }
    
    // Test 3: Memory reading
    std::cout << "\nTest 3: Memory Reading" << std::endl;
    auto* provider = m_scanner->getMemoryProvider();
    if (provider) {
        uint8_t buffer[4];
        if (provider->readMemory(0x500000, buffer, sizeof(buffer))) {
            std::cout << "✓ Memory read successful" << std::endl;
            std::cout << "  Read 4 bytes: ";
            for (int i = 0; i < 4; ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                         << static_cast<int>(buffer[i]) << " ";
            }
            std::cout << std::dec << std::endl;
        } else {
            std::cout << "✗ Memory read failed" << std::endl;
        }
    }
    
    std::cout << "\n=== Tests Complete ===" << std::endl;
}

} // namespace ui