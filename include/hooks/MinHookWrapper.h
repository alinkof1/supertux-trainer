#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace hooks {

/**
 * @brief Status codes for MinHook operations
 */
enum class MHStatus {
    MH_OK = 0,
    MH_ERROR_ALREADY_INITIALIZED,
    MH_ERROR_NOT_INITIALIZED,
    MH_ERROR_ALREADY_CREATED,
    MH_ERROR_NOT_CREATED,
    MH_ERROR_ENABLED,
    MH_ERROR_DISABLED,
    MH_ERROR_NOT_EXECUTABLE,
    MH_ERROR_UNSUPPORTED_FUNCTION,
    MH_ERROR_MEMORY_ALLOC,
    MH_ERROR_MEMORY_PROTECT,
    MH_ERROR_MODULE_NOT_FOUND,
    MH_ERROR_FUNCTION_NOT_FOUND
};

/**
 * @brief Hook type
 */
enum class HookType {
    HOOK_JMP,
    HOOK_CALL,
    HOOK_VTABLE
};

/**
 * @brief Function hook information
 */
struct HookInfo {
    std::string name;
    uintptr_t targetAddress;
    uintptr_t hookFunction;
    uintptr_t originalFunction;
    HookType type;
    bool enabled;
    
    HookInfo(const std::string& n, uintptr_t target, uintptr_t hook, HookType t = HookType::HOOK_JMP)
        : name(n), targetAddress(target), hookFunction(hook), originalFunction(0), 
          type(t), enabled(false) {}
};

/**
 * @brief Wrapper for MinHook library
 */
class MinHookWrapper {
public:
    /**
     * @brief Initialize MinHook
     */
    static MHStatus initialize();
    
    /**
     * @brief Uninitialize MinHook
     */
    static MHStatus uninitialize();
    
    /**
     * @brief Create a hook for a function
     * 
     * @param target Address of function to hook
     * @param hook Address of hook function
     * @param original Output parameter for original function pointer
     * @return MHStatus
     */
    static MHStatus createHook(void* target, void* hook, void** original);
    
    /**
     * @brief Create a hook for a function by address
     */
    static MHStatus createHook(uintptr_t target, uintptr_t hook, uintptr_t* original);
    
    /**
     * @brief Enable a hook
     */
    static MHStatus enableHook(void* target);
    
    /**
     * @brief Disable a hook
     */
    static MHStatus disableHook(void* target);
    
    /**
     * @brief Remove a hook
     */
    static MHStatus removeHook(void* target);
    
    /**
     * @brief Check if MinHook is initialized
     */
    static bool isInitialized();
    
    /**
     * @brief Get last error message
     */
    static std::string getLastError();
    
    /**
     * @brief Create trampoline function
     */
    static uintptr_t createTrampoline(uintptr_t target, size_t stolenBytes = 5);
    
private:
    static bool s_initialized;
    static std::string s_lastError;
    
    // Mock implementation for demonstration
    static std::vector<HookInfo> s_hooks;
};

/**
 * @brief Manages a single function hook
 */
class FunctionHook {
public:
    /**
     * @brief Construct a function hook
     * 
     * @param name Name of the hook
     * @param targetAddress Address to hook
     * @param hookFunction Function to call instead
     * @param type Type of hook
     */
    FunctionHook(const std::string& name, uintptr_t targetAddress, 
                 uintptr_t hookFunction, HookType type = HookType::HOOK_JMP);
    
    /**
     * @brief Destructor - automatically removes hook
     */
    ~FunctionHook();
    
    /**
     * @brief Install the hook
     * @return true if successful
     */
    bool install();
    
    /**
     * @brief Remove the hook
     * @return true if successful
     */
    bool remove();
    
    /**
     * @brief Enable the hook
     */
    bool enable();
    
    /**
     * @brief Disable the hook
     */
    bool disable();
    
    /**
     * @brief Check if hook is installed
     */
    bool isInstalled() const { return m_installed; }
    
    /**
     * @brief Check if hook is enabled
     */
    bool isEnabled() const { return m_enabled; }
    
    /**
     * @brief Get the original function address
     */
    uintptr_t getOriginal() const { return m_originalFunction; }
    
    /**
     * @brief Call the original function
     */
    template<typename Ret, typename... Args>
    Ret callOriginal(Args... args) {
        using FuncPtr = Ret(*)(Args...);
        FuncPtr originalFunc = reinterpret_cast<FuncPtr>(m_originalFunction);
        return originalFunc(args...);
    }
    
private:
    std::string m_name;
    uintptr_t m_targetAddress;
    uintptr_t m_hookFunction;
    uintptr_t m_originalFunction;
    HookType m_type;
    bool m_installed;
    bool m_enabled;
};

} // namespace hooks