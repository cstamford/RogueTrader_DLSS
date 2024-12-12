#pragma once

#include <Windows.h>
#include <vector>

using FnServiceInitialize = bool (*)();
using FnServiceGetHook = FARPROC (*)(const char*);
using FnServiceSetHookOriginal = void (*)(const char*, FARPROC);

struct ServiceFlags {
    enum Enum {
        None = 0,
        DebugOnly = 1 << 0,
    };
};

struct ServiceInfo {
    const char* Name;
    enum ServiceFlags::Enum Flags = ServiceFlags::None;
    FnServiceInitialize FnInitialize = nullptr;
    FnServiceGetHook FnGetHook = nullptr;
    FnServiceSetHookOriginal FnSetHookOriginal = nullptr;
};

void InitializeServices();
void RegisterService(ServiceInfo&& info);
const std::vector<ServiceInfo>& GetServices();

struct ServiceRegistrar {
    ServiceRegistrar(ServiceInfo&& info);
};

#define SERVICE_CONCAT(a, b) SERVICE_CONCAT_INNER(a, b)
#define SERVICE_CONCAT_INNER(a, b) a##b
#define REGISTER_SERVICE(...)                                                \
    static ServiceRegistrar SERVICE_CONCAT(_serviceRegistrar_, __COUNTER__)( \
        []() -> ServiceInfo {                                                \
            return ServiceInfo { __VA_ARGS__ };                              \
        }())


