#include "scanner/PatternScanner.h"
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <memory>

namespace scanner {

class WindowsMemoryProvider : public IMemoryProvider {
public:
    WindowsMemoryProvider(DWORD processId) : m_processId(processId), m_hProcess(nullptr) {
        m_hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);
    }
    
    ~WindowsMemoryProvider() override {
        if (m_hProcess) {
            CloseHandle(m_hProcess);
        }
    }
    
    bool readMemory(uintptr_t address, void* buffer, size_t size) override {
        if (!m_hProcess) return false;
        
        SIZE_T bytesRead = 0;
        return ReadProcessMemory(m_hProcess, reinterpret_cast<LPCVOID>(address), 
                                buffer, size, &bytesRead) && bytesRead == size;
    }
    
    uintptr_t getModuleBase(const std::string& moduleName) override {
        if (!m_hProcess) return 0;
        
        HMODULE hModules[1024];
        DWORD cbNeeded;
        
        if (EnumProcessModules(m_hProcess, hModules, sizeof(hModules), &cbNeeded)) {
            for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                char szModuleName[MAX_PATH];
                if (GetModuleFileNameExA(m_hProcess, hModules[i], szModuleName, 
                                        sizeof(szModuleName))) {
                    std::string currentModule(szModuleName);
                    size_t pos = currentModule.find_last_of("\\/");
                    if (pos != std::string::npos) {
                        currentModule = currentModule.substr(pos + 1);
                    }
                    
                    if (_stricmp(currentModule.c_str(), moduleName.c_str()) == 0) {
                        return reinterpret_cast<uintptr_t>(hModules[i]);
                    }
                }
            }
        }
        
        return 0;
    }
    
    size_t getModuleSize(const std::string& moduleName) override {
        if (!m_hProcess) return 0;
        
        uintptr_t baseAddress = getModuleBase(moduleName);
        if (baseAddress == 0) return 0;
        
        MODULEINFO moduleInfo;
        if (GetModuleInformation(m_hProcess, reinterpret_cast<HMODULE>(baseAddress), 
                                &moduleInfo, sizeof(moduleInfo))) {
            return moduleInfo.SizeOfImage;
        }
        
        return 0;
    }
    
    bool isValidAddress(uintptr_t address) override {
        if (!m_hProcess) return false;
        
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQueryEx(m_hProcess, reinterpret_cast<LPCVOID>(address), 
                          &mbi, sizeof(mbi)) == 0) {
            return false;
        }
        
        return (mbi.State == MEM_COMMIT) && 
               (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | 
                              PAGE_EXECUTE_READWRITE)) != 0;
    }
    
    static DWORD findProcessId(const std::string& processName) {
        DWORD processId = 0;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);
            
            if (Process32First(hSnapshot, &pe32)) {
                do {
                    if (_stricmp(pe32.szExeFile, processName.c_str()) == 0) {
                        processId = pe32.th32ProcessID;
                        break;
                    }
                } while (Process32Next(hSnapshot, &pe32));
            }
            
            CloseHandle(hSnapshot);
        }
        
        return processId;
    }
    
private:
    DWORD m_processId;
    HANDLE m_hProcess;
};

} // namespace scanner