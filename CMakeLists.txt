cmake_minimum_required(VERSION 3.10)
project(GameTrainer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cross-compilation settings for Windows
if(CMAKE_CROSSCOMPILING)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
endif()

# Include directories
include_directories(include)
include_directories(lib)

# Source files
set(SOURCES
    src/main.cpp
    src/scanner/PatternScanner.cpp
    src/memory/Pattern.cpp
    src/memory/MockMemoryProvider.cpp
    src/hooks/MinHookWrapper.cpp
    src/ui/ConsoleUI.cpp
)

# Create executable
add_executable(game-trainer ${SOURCES})

# Target properties
set_target_properties(game-trainer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Link libraries (MinHook will be added manually)
target_link_libraries(game-trainer)