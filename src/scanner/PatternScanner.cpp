#include "scanner/PatternScanner.h"
#include <algorithm>
#include <cstring>

namespace scanner {

PatternScanner::PatternScanner(std::unique_ptr<IMemoryProvider> memoryProvider)
    : m_memoryProvider(std::move(memoryProvider)) {}

bool PatternScanner::scanSingle(
    const memory::Pattern& pattern,
    uintptr_t startAddress,
    size_t size,
    memory::PatternResult& result) {
    
    if (!m_memoryProvider || !m_memoryProvider->isValidAddress(startAddress)) {
        return false;
    }
    
    if (m_useBoyerMoore) {
        return boyerMooreScan(pattern, startAddress, size, result);
    } else {
        return naiveScan(pattern, startAddress, size, result);
    }
}

bool PatternScanner::scanModule(
    const memory::Pattern& pattern,
    const std::string& moduleName,
    memory::PatternResult& result) {
    
    uintptr_t baseAddress = m_memoryProvider->getModuleBase(moduleName);
    if (baseAddress == 0) {
        return false;
    }
    
    size_t moduleSize = m_memoryProvider->getModuleSize(moduleName);
    if (moduleSize == 0) {
        return false;
    }
    
    return scanSingle(pattern, baseAddress, moduleSize, result);
}

std::vector<memory::PatternResult> PatternScanner::scanMultiple(
    const std::vector<memory::Pattern>& patterns,
    uintptr_t startAddress,
    size_t size) {
    
    std::vector<memory::PatternResult> results;
    
    for (const auto& pattern : patterns) {
        memory::PatternResult result;
        if (scanSingle(pattern, startAddress, size, result)) {
            results.push_back(result);
        }
    }
    
    return results;
}

bool PatternScanner::scanEntireProcess(
    const memory::Pattern& pattern,
    memory::PatternResult& result) {
    
    // In a real implementation, we would enumerate all memory regions
    // For now, scan the main module
    return scanModule(pattern, "supertux.exe", result);
}

bool PatternScanner::naiveScan(
    const memory::Pattern& pattern,
    uintptr_t startAddress,
    size_t size,
    memory::PatternResult& result) {
    
    // Read the memory region
    std::vector<uint8_t> memory = readMemoryRegion(startAddress, size);
    if (memory.empty()) {
        return false;
    }
    
    const size_t patternSize = pattern.size();
    if (patternSize > memory.size()) {
        return false;
    }
    
    // Naive scanning: check every possible position
    for (size_t i = 0; i <= memory.size() - patternSize; ++i) {
        if (pattern.matches(&memory[i])) {
            result.address = startAddress + i;
            result.patternName = pattern.getName();
            result.matchedBytes.assign(memory.begin() + i, memory.begin() + i + patternSize);
            return true;
        }
    }
    
    return false;
}

bool PatternScanner::boyerMooreScan(
    const memory::Pattern& pattern,
    uintptr_t startAddress,
    size_t size,
    memory::PatternResult& result) {
    
    // Simplified Boyer-Moore implementation for patterns with wildcards
    // For demonstration, we'll use the naive scan
    // In a real implementation, we would implement proper Boyer-Moore
    return naiveScan(pattern, startAddress, size, result);
}

std::vector<uint8_t> PatternScanner::readMemoryRegion(uintptr_t address, size_t size) {
    std::vector<uint8_t> buffer(size);
    
    if (m_memoryProvider->readMemory(address, buffer.data(), size)) {
        return buffer;
    }
    
    return {};
}

} // namespace scanner