#include "scanner/PatternScanner.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cstring>

namespace scanner {

/**
 * @brief Mock memory provider for demonstration and testing
 * 
 * This provider simulates memory reading without requiring Windows APIs
 */
class MockMemoryProvider : public IMemoryProvider {
public:
    MockMemoryProvider() {
        // Initialize with some mock memory data for demonstration
        initializeMockMemory();
    }
    
    bool readMemory(uintptr_t address, void* buffer, size_t size) override {
        auto it = m_memoryRegions.lower_bound(address);
        
        if (it == m_memoryRegions.end()) {
            return false;
        }
        
        // Check if the requested range is within a region
        uintptr_t regionStart = it->first;
        const std::vector<uint8_t>& regionData = it->second;
        
        if (address < regionStart || address + size > regionStart + regionData.size()) {
            return false;
        }
        
        // Copy data from mock memory
        size_t offset = address - regionStart;
        std::memcpy(buffer, regionData.data() + offset, size);
        return true;
    }
    
    uintptr_t getModuleBase(const std::string& moduleName) override {
        auto it = m_moduleBases.find(moduleName);
        if (it != m_moduleBases.end()) {
            return it->second;
        }
        return 0;
    }
    
    size_t getModuleSize(const std::string& moduleName) override {
        auto it = m_moduleSizes.find(moduleName);
        if (it != m_moduleSizes.end()) {
            return it->second;
        }
        return 0;
    }
    
    bool isValidAddress(uintptr_t address) override {
        for (const auto& region : m_memoryRegions) {
            if (address >= region.first && 
                address < region.first + region.second.size()) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * @brief Add mock memory region
     */
    void addMemoryRegion(uintptr_t baseAddress, const std::vector<uint8_t>& data) {
        m_memoryRegions[baseAddress] = data;
    }
    
    /**
     * @brief Add mock module
     */
    void addModule(const std::string& name, uintptr_t baseAddress, size_t size) {
        m_moduleBases[name] = baseAddress;
        m_moduleSizes[name] = size;
    }
    
private:
    std::map<uintptr_t, std::vector<uint8_t>> m_memoryRegions;
    std::map<std::string, uintptr_t> m_moduleBases;
    std::map<std::string, size_t> m_moduleSizes;
    
    void initializeMockMemory() {
        // Add a mock module "supertux.exe" at address 0x400000
        const uintptr_t supertuxBase = 0x400000;
        const size_t supertuxSize = 0x100000;
        
        addModule("supertux.exe", supertuxBase, supertuxSize);
        
        // Create mock memory data with some patterns for testing
        std::vector<uint8_t> mockMemory(supertuxSize, 0x90); // Fill with NOPs
        
        // Add some test patterns
        // Pattern 1: Health variable access (simulated)
        // mov eax, [health_ptr]
        uintptr_t healthPatternAddr = supertuxBase + 0x12345;
        mockMemory[0x12345] = 0x8B;  // mov eax
        mockMemory[0x12346] = 0x05;  // [health_ptr]
        mockMemory[0x12347] = 0x78;  // ...
        mockMemory[0x12348] = 0x56;  // ...
        mockMemory[0x12349] = 0x34;  // ...
        mockMemory[0x1234A] = 0x12;  // ...
        
        // Pattern 2: Coin count update (simulated)
        // add [coin_count], ebx
        uintptr_t coinPatternAddr = supertuxBase + 0x23456;
        mockMemory[0x23456] = 0x01;  // add
        mockMemory[0x23457] = 0x1D;  // [coin_count]
        mockMemory[0x23458] = 0xBC;  // ...
        mockMemory[0x23459] = 0x9A;  // ...
        mockMemory[0x2345A] = 0x78;  // ...
        mockMemory[0x2345B] = 0x56;  // ...
        
        // Pattern 3: Function prologue for hooking
        // push ebp; mov ebp, esp
        uintptr_t funcPatternAddr = supertuxBase + 0x34567;
        mockMemory[0x34567] = 0x55;  // push ebp
        mockMemory[0x34568] = 0x8B;  // mov ebp, esp
        mockMemory[0x34569] = 0xEC;  // ...
        
        addMemoryRegion(supertuxBase, mockMemory);
        
        // Add another region for heap data
        std::vector<uint8_t> heapData(0x10000, 0x00);
        // Simulate some game data
        uintptr_t healthAddr = 0x500000;
        *reinterpret_cast<uint32_t*>(&heapData[0x1000]) = 100; // Health = 100
        *reinterpret_cast<uint32_t*>(&heapData[0x1004]) = 50;  // Coins = 50
        
        addMemoryRegion(0x500000, heapData);
    }
};

} // namespace scanner