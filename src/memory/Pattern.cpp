#include "memory/Pattern.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace memory {

Pattern::Pattern(const std::string& patternString, const std::string& name)
    : m_name(name) {
    parsePatternString(patternString);
}

Pattern::Pattern(const std::vector<uint8_t>& bytes, const std::vector<bool>& mask, 
                 const std::string& name)
    : m_bytes(bytes), m_mask(mask), m_name(name) {
    if (m_bytes.size() != m_mask.size()) {
        throw std::invalid_argument("Byte array and mask must have same size");
    }
}

void Pattern::parsePatternString(const std::string& patternString) {
    std::istringstream iss(patternString);
    std::string token;
    
    while (iss >> token) {
        if (token == "??") {
            // Wildcard byte
            m_bytes.push_back(0x00);
            m_mask.push_back(false);
        } else {
            // Parse hex byte
            try {
                uint8_t byte = static_cast<uint8_t>(std::stoul(token, nullptr, 16));
                m_bytes.push_back(byte);
                m_mask.push_back(true);
            } catch (const std::exception&) {
                throw std::invalid_argument("Invalid pattern token: " + token);
            }
        }
    }
    
    if (m_bytes.empty()) {
        throw std::invalid_argument("Pattern string cannot be empty");
    }
}

bool Pattern::isWildcard(size_t pos) const {
    if (pos >= m_mask.size()) {
        throw std::out_of_range("Position out of range");
    }
    return !m_mask[pos];
}

std::string Pattern::toString() const {
    std::ostringstream oss;
    for (size_t i = 0; i < m_bytes.size(); ++i) {
        if (i > 0) oss << " ";
        if (m_mask[i]) {
            oss << std::hex << std::setw(2) << std::setfill('0') 
                << static_cast<int>(m_bytes[i]);
        } else {
            oss << "??";
        }
    }
    return oss.str();
}

bool Pattern::matches(const uint8_t* data) const {
    for (size_t i = 0; i < m_bytes.size(); ++i) {
        if (m_mask[i] && data[i] != m_bytes[i]) {
            return false;
        }
    }
    return true;
}

uintptr_t PatternResult::calculateOffset(int offset) const {
    return address + offset;
}

bool PatternResult::readPointerChain(const std::vector<int>& offsets, uintptr_t& result) const {
    // This would need actual memory reading implementation
    // For now, return a dummy implementation
    if (offsets.empty()) {
        result = address;
        return true;
    }
    
    // Simulate pointer chain resolution
    uintptr_t current = address;
    for (int offset : offsets) {
        current += offset;
        // In real implementation, we would read memory here
    }
    
    result = current;
    return true;
}

} // namespace memory