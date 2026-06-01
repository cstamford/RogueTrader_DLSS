#include <Services.hpp>
#include <Util.hpp>

#include "Services/Platform/DXGI/_DXGI.hpp"

using FnGetProcAddress = FARPROC(WINAPI*)(HMODULE, LPCSTR);
FnGetProcAddress _GetProcAddress;
FARPROC WINAPI Hook_GetProcAddress(_In_ HMODULE hModule, _In_ LPCSTR lpProcName);

static void SetupConsole();
void RegisterDirectHooks();

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID _)
{
    if (reason == DLL_PROCESS_ATTACH) {
#ifdef DEBUG
        SetupConsole();
#endif

        InitializeServices();
    
        if (FAIL(MH_Initialize())) {
            return false;
        }

        if (FAIL(MH_CreateHookApi(L"kernel32.dll", "GetProcAddress", &Hook_GetProcAddress, (LPVOID*)&_GetProcAddress))) {
            return false;
        }
        
        DXGI::RegisterHooks();
        RegisterDirectHooks();

        if (FAIL(MH_EnableHook(MH_ALL_HOOKS))) {
            return false;
        }

        LOG_INFO("DLSS_Native loaded!\n");
    }

    return TRUE;
}

FARPROC WINAPI Hook_GetProcAddress(
    _In_ HMODULE hModule,
    _In_ LPCSTR lpProcName)
{
    const bool isOrdinal = HIWORD(lpProcName) == 0;

    if (isOrdinal) {
        return _GetProcAddress(hModule, lpProcName);
    }

    for (const ServiceInfo& service : GetServices()) {
        if (!service.FnGetHook) {
            continue;
        }

        if (FARPROC proc = service.FnGetHook(lpProcName)) {
            if (service.FnSetHookOriginal) {
                service.FnSetHookOriginal(lpProcName, _GetProcAddress(hModule, lpProcName));
            }

            // LOG_DEBUGV("API redirected from {} to service {}\n", lpProcName, service.Name);
            return proc;
        }
    }

    return _GetProcAddress(hModule, lpProcName);
}

static void SetupConsole()
{
    AllocConsole();

    (void)freopen("CONOUT$", "w", stdout);
    (void)freopen("CONOUT$", "w", stderr);

    HANDLE handle = CHECK(CreateFile(L"CONOUT$",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr));

    DISCARD(SetStdHandle(STD_OUTPUT_HANDLE, handle));
    DISCARD(SetStdHandle(STD_ERROR_HANDLE, handle));

    DWORD orig = 0;
    DISCARD(GetConsoleMode(handle, &orig));

    if (FAIL(SetConsoleMode(handle, orig | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN))) {
        DISCARD(SetConsoleMode(handle, orig | ENABLE_VIRTUAL_TERMINAL_PROCESSING));
    }
}

void RegisterDirectHooks()
{
    for (const ServiceInfo& service : GetServices())
    {
        for (const DirectHook& hook : service.DirectHooks) {
            LPVOID trampoline = nullptr;
            if (FAIL(MH_CreateHookApi(hook.Module, hook.FunctionName, hook.HookFunction, &trampoline))) {
                LOG_ERROR("Failed to create hook for {}!\n", hook.FunctionName);
                continue;
            }
            
            if (service.FnSetHookOriginal) {
                service.FnSetHookOriginal(hook.FunctionName, reinterpret_cast<FARPROC>(trampoline));
            }
            
            LOG_DEBUGV("Direct hook registered for {}\n", hook.FunctionName);
        }
    }
}
