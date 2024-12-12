#include <Services.hpp>
#include <Util.hpp>

extern "C" std::byte SteamAPI_RestartAppIfNecessary(uint32_t appId)
{
    return std::byte(1);
}

extern "C" std::byte SteamAPI_Init()
{
    return std::byte(1);
}

static FARPROC GetHook(const char* fn)
{
    if (strcmp(fn, "SteamAPI_RestartAppIfNecessary") == 0) {
        //return (FARPROC)SteamAPI_RestartAppIfNecessary;
    }

    if (strcmp(fn, "SteamAPI_Init") == 0) {
        //return (FARPROC)SteamAPI_Init;
    }

    return nullptr;
}

REGISTER_SERVICE(
    .Name = "DisableSteamAppRestart",
    .Flags = ServiceFlags::DebugOnly,
    .FnGetHook = GetHook
);
