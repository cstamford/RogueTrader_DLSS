#include <Services.hpp>
#include <Util.hpp>

static void DebugPrintFromManaged(const char* str)
{
    LOG_DEBUG("{}", str);
}

static FARPROC GetHook(const char* fn)
{
    if (strcmp(fn, "DebugPrintFromManaged") == 0) {
        return (FARPROC)DebugPrintFromManaged;
    }

    return nullptr;
}

REGISTER_SERVICE(
    .Name = "DebugPrintFromManaged",
    .Flags = ServiceFlags::DebugOnly,
    .FnGetHook = GetHook
);
