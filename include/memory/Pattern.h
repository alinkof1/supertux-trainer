#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace memory {

/**
 * @brief Represents a byte pattern for memory scanning
 * 
 * Supports wildcard bytes (represented as ??) and mask-based matching
 */
class Pattern {
public:
    /**
     * @brief Construct a pattern from a string representation
     * 
     * @param patternString String like "48 8B 05 ?? ?? ?? ?? 48 85 C0"
     * @param name Optional name for the pattern
     */
    Pattern(const std::string& patternString, const std::string& name = "");
    
    /**
     * @brief Construct a pattern from byte array and mask
     * 
     * @param bytes Byte values
     * @param mask Mask where true = must match, false = wildcard
     * @param name Optional name for the pattern
     */
    Pattern(const std::vector<uint8_t>& bytes, const std::vector<bool>& mask, 
            const std::string& name = "");
    
    /**
     * @brief Get the pattern bytes
     */
    const std::vector<uint8_t>& getBytes() const { return m_bytes; }
    
    /**
     * @brief Get the pattern mask
     */
    const std::vector<bool>& getMask() const { return m_mask; }
    
    /**
     * @brief Get the pattern name
     */
    const std::string& getName() const { return m_name; }
    
    /**
     * @brief Get the pattern size in bytes
     */
    size_t size() const { return m_bytes.size(); }
    
    /**
     * @brief Check if a position is a wildcard
     */
    bool isWildcard(size_t pos) const;
    
    /**
     * @brief Convert pattern to string representation
     */
    std::string toString() const;
    
    /**
     * @brief Match pattern against memory
     * 
     * @param data Pointer to memory to check
     * @return true if pattern matches
     */
    bool matches(const uint8_t* data) const;
    
private:
    std::vector<uint8_t> m_bytes;
    std::vector<bool> m_mask;
    std::string m_name;
    
    void parsePatternString(const std::string& patternString);
};

/**
 * @brief Result of a pattern scan
 */
struct PatternResult {
    uintptr_t address;
    std::string patternName;
    std::vector<uint8_t> matchedBytes;
    
    PatternResult(uintptr_t addr = 0, const std::string& name = "")
        : address(addr), patternName(name) {}
    
    /**
     * @brief Calculate relative offset from this result
     */
    uintptr_t calculateOffset(int offset) const;
    
    /**
     * @brief Read pointer chain from this address
     * @param offsets Vector of offsets to follow
     * @param result Output parameter for the final address
     * @return true if successful, false otherwise
     */
    bool readPointerChain(const std::vector<int>& offsets, uintptr_t& result) const;
};

} // namespace memory