# Game Trainer for SuperTux

A complete game mod implementation featuring binary pattern matching, offset calculation, and MinHook integration for creating a basic trainer against the open-source game SuperTux.

## Features Implemented

### 1. Binary Pattern Matching Tool
- **Pattern Scanner**: Scans memory for byte patterns with wildcard support
- **Multiple Algorithms**: Supports both naive scanning and Boyer-Moore (simplified)
- **Wildcard Support**: Patterns can include `??` for variable bytes
- **Pattern Management**: Create, store, and manage patterns for different game versions

### 2. Offset Calculation System
- **Relative Offsets**: Calculate addresses relative to pattern matches
- **Pointer Chains**: Resolve multi-level pointer chains to find actual values
- **Base Address Calculation**: Handle ASLR by calculating offsets from module bases

### 3. MinHook Integration
- **Function Hooking**: Intercept game functions using MinHook library
- **Trampoline Support**: Call original functions after hooking
- **Hook Management**: Install, enable, disable, and remove hooks dynamically
- **Error Handling**: Comprehensive error reporting for hook operations

### 4. Basic Trainer Functionality
- **Console Interface**: Interactive command-line interface for trainer operations
- **Memory Reading/Writing**: Read and modify game memory
- **Pattern Database**: Pre-defined patterns for SuperTux game variables
- **Hook Examples**: Demonstration hooks for health, coins, and lives

## Project Structure

```
game-trainer/
├── include/                 # Header files
│   ├── memory/             # Memory pattern and scanning
│   ├── scanner/            # Pattern scanner interface
│   ├── hooks/              # MinHook wrapper and function hooks
│   └── ui/                 # User interface
├── src/                    # Source files
│   ├── memory/             # Pattern implementation
│   ├── scanner/            # Scanner implementation
│   ├── hooks/              # Hook implementation
│   └── ui/                 # Console UI implementation
├── CMakeLists.txt          # Build configuration
└── README.md               # This file
```

## Key Components

### Pattern Scanner (`PatternScanner`)
- Scans process memory for byte patterns
- Supports wildcards for variable bytes
- Can scan specific modules or entire process
- Returns addresses of pattern matches

### Mock Memory Provider (`MockMemoryProvider`)
- Simulates game memory for testing
- Contains pre-defined patterns for SuperTux
- Allows testing without actual game process

### MinHook Wrapper (`MinHookWrapper`)
- Wrapper around MinHook library for Windows
- Manages hook creation, enabling, and removal
- Provides error handling and status reporting

### Console UI (`ConsoleUI`)
- Interactive command-line interface
- Commands for scanning, hooking, and memory operations
- Demonstration tests for all features

## Usage Examples

### 1. Pattern Scanning
```cpp
memory::Pattern healthPattern("8B 05 ?? ?? ?? ??", "Health Access");
memory::PatternResult result;

if (scanner.scanEntireProcess(healthPattern, result)) {
    std::cout << "Health pattern found at: 0x" << std::hex << result.address << std::endl;
}
```

### 2. Function Hooking
```cpp
hooks::FunctionHook healthHook("health_hook", 0x12345678, 
                               reinterpret_cast<uintptr_t>(&health_hook_function));
                               
if (healthHook.install() && healthHook.enable()) {
    std::cout << "Health hook installed and enabled" << std::endl;
}
```

### 3. Console Commands
```
> scan "8B 05 ?? ?? ?? ??"
> patterns
> hook 0x12345678 health_hook
> hooks
> memory 0x500000
> test
```

## Building the Project

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler
- mingw-w64 (for Windows cross-compilation)

### Build Instructions
```bash
# Create build directory
mkdir build && cd build

# Configure for cross-compilation to Windows
cmake .. -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
         -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
         -DCMAKE_CROSSCOMPILING=ON

# Build the project
cmake --build .
```

### Testing
```bash
# Compile and run simple test
g++ -std=c++17 -I./include -o test_simple test_simple.cpp src/memory/Pattern.cpp
./test_simple
```

## Integration with SuperTux

### Target Variables
The trainer is designed to work with SuperTux game variables:
- **Health**: Player health/lives
- **Coins**: Coin count
- **Position**: Level position
- **Game State**: Current game state flags

### Pattern Discovery
To find patterns for SuperTux:
1. Analyze SuperTux executable with disassembler
2. Identify instructions accessing game variables
3. Create patterns with wildcards for variable addresses
4. Test patterns against different game versions

### Hooking Strategy
1. Scan for function prologues (`55 8B EC`)
2. Hook functions that modify game state
3. Implement hook functions to modify behavior
4. Enable hooks to intercept game execution

## Extending the Trainer

### Adding New Patterns
1. Add patterns to `MockMemoryProvider::initializeMockMemory()`
2. Update console commands in `ConsoleUI::showPatterns()`
3. Test patterns with the scanner

### Adding New Hooks
1. Create hook function with desired behavior
2. Add hook creation to console commands
3. Implement hook management in UI

### Supporting Other Games
1. Update mock memory with game-specific patterns
2. Modify module names and addresses
3. Adjust patterns for different game architectures

## Limitations and Considerations

### Current Implementation
- Uses mock memory provider for demonstration
- Simplified Boyer-Moore algorithm
- Mock MinHook implementation (not actual Windows hooks)

### Production Use
For actual game training:
1. Replace `MockMemoryProvider` with `WindowsMemoryProvider`
2. Use actual MinHook library for Windows
3. Discover real patterns for target game version
4. Implement proper error handling and logging

### Cross-Platform Considerations
- MinHook is Windows-specific
- Memory reading APIs differ by platform
- Pattern scanning algorithms are platform-agnostic

## License

This project is for educational purposes to demonstrate game modding techniques. Use responsibly and only on games you own or have permission to modify.

## Acknowledgments

- **SuperTux**: Open-source 2D platformer used as target game
- **MinHook**: Minimalistic API hooking library for Windows
- **CMake**: Cross-platform build system