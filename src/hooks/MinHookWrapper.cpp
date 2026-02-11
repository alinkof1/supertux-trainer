#include "hooks/MinHookWrapper.h"
#include <algorithm>
#include <sstream>

namespace hooks {

// Static member initialization
bool MinHookWrapper::s_initialized = false;
std::string MinHookWrapper::s_lastError = "";
std::vector<HookInfo> MinHookWrapper::s_hooks;

MHStatus MinHookWrapper::initialize() {
    if (s_initialized) {
        s_lastError = "MinHook already initialized";
        return MHStatus::MH_ERROR_ALREADY_INITIALIZED;
    }
    
    s_initialized = true;
    s_lastError = "Initialized successfully (mock implementation)";
    return MHStatus::MH_OK;
}

MHStatus MinHookWrapper::uninitialize() {
    if (!s_initialized) {
        s_lastError = "MinHook not initialized";
        return MHStatus::MH_ERROR_NOT_INITIALIZED;
    }
    
    // Remove all hooks
    for (auto& hook : s_hooks) {
        if (hook.enabled) {
            disableHook(reinterpret_cast<void*>(hook.targetAddress));
        }
    }
    s_hooks.clear();
    
    s_initialized = false;
    s_lastError = "Uninitialized successfully";
    return MHStatus::MH_OK;
}

MHStatus MinHookWrapper::createHook(void* target, void* hook, void** original) {
    if (!s_initialized) {
        s_lastError = "MinHook not initialized";
        return MHStatus::MH_ERROR_NOT_INITIALIZED;
    }
    
    uintptr_t targetAddr = reinterpret_cast<uintptr_t>(target);
    uintptr_t hookAddr = reinterpret_cast<uintptr_t>(hook);
    
    // Check if hook already exists
    auto it = std::find_if(s_hooks.begin(), s_hooks.end(),
        [targetAddr](const HookInfo& h) { return h.targetAddress == targetAddr; });
    
    if (it != s_hooks.end()) {
        s_lastError = "Hook already exists for this address";
        return MHStatus::MH_ERROR_ALREADY_CREATED;
    }
    
    // Create mock hook
    HookInfo hookInfo("", targetAddr, hookAddr);
    hookInfo.originalFunction = targetAddr; // In real MinHook, this would be different
    
    s_hooks.push_back(hookInfo);
    
    if (original) {
        *original = reinterpret_cast<void*>(hookInfo.originalFunction);
    }
    
    s_lastError = "Hook created successfully (mock)";
    return MHStatus::MH_OK;
}

MHStatus MinHookWrapper::createHook(uintptr_t target, uintptr_t hook, uintptr_t* original) {
    return createHook(reinterpret_cast<void*>(target), 
                     reinterpret_cast<void*>(hook), 
                     reinterpret_cast<void**>(original));
}

MHStatus MinHookWrapper::enableHook(void* target) {
    if (!s_initialized) {
        s_lastError = "MinHook not initialized";
        return MHStatus::MH_ERROR_NOT_INITIALIZED;
    }
    
    uintptr_t targetAddr = reinterpret_cast<uintptr_t>(target);
    
    auto it = std::find_if(s_hooks.begin(), s_hooks.end(),
        [targetAddr](const HookInfo& h) { return h.targetAddress == targetAddr; });
    
    if (it == s_hooks.end()) {
        s_lastError = "Hook not found";
        return MHStatus::MH_ERROR_NOT_CREATED;
    }
    
    if (it->enabled) {
        s_lastError = "Hook already enabled";
        return MHStatus::MH_ERROR_ENABLED;
    }
    
    it->enabled = true;
    s_lastError = "Hook enabled successfully (mock)";
    return MHStatus::MH_OK;
}

MHStatus MinHookWrapper::disableHook(void* target) {
    if (!s_initialized) {
        s_lastError = "MinHook not initialized";
        return MHStatus::MH_ERROR_NOT_INITIALIZED;
    }
    
    uintptr_t targetAddr = reinterpret_cast<uintptr_t>(target);
    
    auto it = std::find_if(s_hooks.begin(), s_hooks.end(),
        [targetAddr](const HookInfo& h) { return h.targetAddress == targetAddr; });
    
    if (it == s_hooks.end()) {
        s_lastError = "Hook not found";
        return MHStatus::MH_ERROR_NOT_CREATED;
    }
    
    if (!it->enabled) {
        s_lastError = "Hook already disabled";
        return MHStatus::MH_ERROR_DISABLED;
    }
    
    it->enabled = false;
    s_lastError = "Hook disabled successfully (mock)";
    return MHStatus::MH_OK;
}

MHStatus MinHookWrapper::removeHook(void* target) {
    if (!s_initialized) {
        s_lastError = "MinHook not initialized";
        return MHStatus::MH_ERROR_NOT_INITIALIZED;
    }
    
    uintptr_t targetAddr = reinterpret_cast<uintptr_t>(target);
    
    auto it = std::find_if(s_hooks.begin(), s_hooks.end(),
        [targetAddr](const HookInfo& h) { return h.targetAddress == targetAddr; });
    
    if (it == s_hooks.end()) {
        s_lastError = "Hook not found";
        return MHStatus::MH_ERROR_NOT_CREATED;
    }
    
    if (it->enabled) {
        disableHook(target);
    }
    
    s_hooks.erase(it);
    s_lastError = "Hook removed successfully";
    return MHStatus::MH_OK;
}

bool MinHookWrapper::isInitialized() {
    return s_initialized;
}

std::string MinHookWrapper::getLastError() {
    return s_lastError;
}

uintptr_t MinHookWrapper::createTrampoline(uintptr_t target, size_t stolenBytes) {
    // Mock implementation - just return the target address
    // In real MinHook, this would allocate executable memory and create a trampoline
    return target;
}

// FunctionHook implementation
FunctionHook::FunctionHook(const std::string& name, uintptr_t targetAddress, 
                         uintptr_t hookFunction, HookType type)
    : m_name(name), m_targetAddress(targetAddress), m_hookFunction(hookFunction),
      m_originalFunction(0), m_type(type), m_installed(false), m_enabled(false) {}

FunctionHook::~FunctionHook() {
    if (m_installed) {
        remove();
    }
}

bool FunctionHook::install() {
    if (m_installed) {
        return true;
    }
    
    MHStatus status = MinHookWrapper::createHook(m_targetAddress, m_hookFunction, &m_originalFunction);
    if (status == MHStatus::MH_OK) {
        m_installed = true;
        return true;
    }
    
    return false;
}

bool FunctionHook::remove() {
    if (!m_installed) {
        return true;
    }
    
    if (m_enabled) {
        disable();
    }
    
    MHStatus status = MinHookWrapper::removeHook(reinterpret_cast<void*>(m_targetAddress));
    if (status == MHStatus::MH_OK) {
        m_installed = false;
        m_originalFunction = 0;
        return true;
    }
    
    return false;
}

bool FunctionHook::enable() {
    if (!m_installed) {
        return false;
    }
    
    if (m_enabled) {
        return true;
    }
    
    MHStatus status = MinHookWrapper::enableHook(reinterpret_cast<void*>(m_targetAddress));
    if (status == MHStatus::MH_OK) {
        m_enabled = true;
        return true;
    }
    
    return false;
}

bool FunctionHook::disable() {
    if (!m_installed || !m_enabled) {
        return true;
    }
    
    MHStatus status = MinHookWrapper::disableHook(reinterpret_cast<void*>(m_targetAddress));
    if (status == MHStatus::MH_OK) {
        m_enabled = false;
        return true;
    }
    
    return false;
}

} // namespace hooks