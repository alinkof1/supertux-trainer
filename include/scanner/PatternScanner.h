#pragma once

#include "memory/Pattern.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <optional>

namespace scanner {

/**
 * @brief Interface for memory region providers
 */
class IMemoryProvider {
public:
    virtual ~IMemoryProvider() = default;
    
    /**
     * @brief Read memory from a process
     * 
     * @param address Starting address
     * @param buffer Output buffer
     * @param size Number of bytes to read
     * @return true if read successful
     */
    virtual bool readMemory(uintptr_t address, void* buffer, size_t size) = 0;
    
    /**
     * @brief Get base address of a module
     * 
     * @param moduleName Name of the module (e.g., "supertux.exe")
     * @return Base address or 0 if not found
     */
    virtual uintptr_t getModuleBase(const std::string& moduleName) = 0;
    
    /**
     * @brief Get size of a module
     */
    virtual size_t getModuleSize(const std::string& moduleName) = 0;
    
    /**
     * @brief Check if an address is valid
     */
    virtual bool isValidAddress(uintptr_t address) = 0;
};

/**
 * @brief Scans memory for patterns using various algorithms
 */
class PatternScanner {
public:
    /**
     * @brief Construct a pattern scanner with a memory provider
     */
    explicit PatternScanner(std::unique_ptr<IMemoryProvider> memoryProvider);
    
    /**
     * @brief Scan for a single pattern in a memory region
     * 
     * @param pattern Pattern to search for
     * @param startAddress Starting address for scan
     * @param size Size of region to scan
     * @param result Output parameter for result if found
     * @return true if pattern found, false otherwise
     */
    bool scanSingle(
        const memory::Pattern& pattern,
        uintptr_t startAddress,
        size_t size,
        memory::PatternResult& result);
    
    /**
     * @brief Scan for a pattern in a module
     * 
     * @param pattern Pattern to search for
     * @param moduleName Name of module to scan
     * @param result Output parameter for result if found
     * @return true if pattern found, false otherwise
     */
    bool scanModule(
        const memory::Pattern& pattern,
        const std::string& moduleName,
        memory::PatternResult& result);
    
    /**
     * @brief Scan for multiple patterns
     * 
     * @param patterns Vector of patterns to search for
     * @param startAddress Starting address for scan
     * @param size Size of region to scan
     * @return Vector of results (may be empty)
     */
    std::vector<memory::PatternResult> scanMultiple(
        const std::vector<memory::Pattern>& patterns,
        uintptr_t startAddress,
        size_t size);
    
    /**
     * @brief Scan entire process memory for a pattern
     * 
     * @param pattern Pattern to search for
     * @param result Output parameter for result if found
     * @return true if pattern found, false otherwise
     */
    bool scanEntireProcess(
        const memory::Pattern& pattern,
        memory::PatternResult& result);
    
    /**
     * @brief Set scan algorithm
     * 
     * @param useBoyerMoore true for Boyer-Moore, false for naive scan
     */
    void setUseBoyerMoore(bool useBoyerMoore) { m_useBoyerMoore = useBoyerMoore; }
    
    /**
     * @brief Get the memory provider
     */
    IMemoryProvider* getMemoryProvider() const { return m_memoryProvider.get(); }
    
private:
    std::unique_ptr<IMemoryProvider> m_memoryProvider;
    bool m_useBoyerMoore = true;
    
    /**
     * @brief Naive pattern scanning algorithm
     * @param pattern Pattern to search for
     * @param startAddress Starting address for scan
     * @param size Size of region to scan
     * @param result Output parameter for result if found
     * @return true if pattern found, false otherwise
     */
    bool naiveScan(
        const memory::Pattern& pattern,
        uintptr_t startAddress,
        size_t size,
        memory::PatternResult& result);
    
    /**
     * @brief Boyer-Moore pattern scanning algorithm
     * @param pattern Pattern to search for
     * @param startAddress Starting address for scan
     * @param size Size of region to scan
     * @param result Output parameter for result if found
     * @return true if pattern found, false otherwise
     */
    bool boyerMooreScan(
        const memory::Pattern& pattern,
        uintptr_t startAddress,
        size_t size,
        memory::PatternResult& result);
    
    /**
     * @brief Read memory region into buffer
     */
    std::vector<uint8_t> readMemoryRegion(uintptr_t address, size_t size);
};

} // namespace scanner