#include <Services.hpp>
#include <Util.hpp>

std::vector<ServiceInfo> _services;
std::vector<ServiceInfo> _serviceCreationQueue; // static init time

ServiceRegistrar::ServiceRegistrar(ServiceInfo&& info)
{
    _serviceCreationQueue.push_back(std::move(info));
}

void InitializeServices() 
{
    for (ServiceInfo& info : _serviceCreationQueue) {
        RegisterService(std::move(info));
    }

    _serviceCreationQueue.clear();
}

void RegisterService(ServiceInfo&& info)
{
#if !defined(_DEBUG)
    if (info.Flags & ServiceFlags::DebugOnly) {
        LOG_DEBUG("Skipping debug-only service: {}\n", info.Name);
        return;
    }
#endif

    if (!info.FnInitialize || info.FnInitialize()) {
        LOG_INFO("Registered service: {}\n", info.Name);
        _services.push_back(std::move(info));
    }
}

const std::vector<ServiceInfo>& GetServices()
{
    return _services;
}
