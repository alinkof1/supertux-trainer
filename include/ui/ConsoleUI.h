#pragma once

#include <memory>
#include <string>
#include <vector>

// Forward declarations
namespace scanner {
class PatternScanner;
class IMemoryProvider;
}

namespace memory {
struct PatternResult;
}

namespace hooks {
class FunctionHook;
}

namespace ui {

/**
 * @brief Console-based user interface for the game trainer
 */
class ConsoleUI {
public:
    /**
     * @brief Construct console UI with a pattern scanner
     */
    explicit ConsoleUI(std::unique_ptr<scanner::PatternScanner> scanner);
    
    /**
     * @brief Destructor
     */
    ~ConsoleUI();
    
    /**
     * @brief Run the console interface
     */
    void run();
    
private:
    std::unique_ptr<scanner::PatternScanner> m_scanner;
    std::vector<memory::PatternResult> m_scanResults;
    std::vector<std::unique_ptr<hooks::FunctionHook>> m_hooks;
    bool m_running;
    
    /**
     * @brief Process a command from user input
     */
    void processCommand(const std::string& command);
    
    /**
     * @brief Show help information
     */
    void showHelp();
    
    /**
     * @brief Process scan command
     */
    void processScanCommand(std::istringstream& iss);
    
    /**
     * @brief Show available patterns
     */
    void showPatterns();
    
    /**
     * @brief Process hook command
     */
    void processHookCommand(std::istringstream& iss);
    
    /**
     * @brief Show active hooks
     */
    void showHooks();
    
    /**
     * @brief Process memory command
     */
    void processMemoryCommand(std::istringstream& iss);
    
    /**
     * @brief Run demonstration tests
     */
    void runTests();
};

} // namespace ui