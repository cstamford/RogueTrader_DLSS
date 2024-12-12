#pragma once

#include <fmt/format.h>
#include <MinHook.h>
#include <Windows.h>

#include <cassert>
#include <cstdio>

template <typename T>
struct ResultTraits {
    static constexpr bool IsSuccess(T result);
    static constexpr const char* AsString(T result);
};

template <typename T>
class ResultLogger {
public:
    ResultLogger(T result, const char* expr, const char* func, const char* file, int line);

    operator bool() const noexcept;
    operator T() const noexcept;

private:
    T _result;
};

template <typename T> requires std::is_invocable_v<T>
struct ScopedCallback {
    inline ScopedCallback(T&& fn) : _fn(fn) { }
    inline ~ScopedCallback() { _fn(); }

private:
    T _fn;
};

#define AT_END_OF_SCOPE(fn) ScopedCallback _scopedCallback_##__COUNTER__ { fn }

#define _RESULT_LOGGER(f) ResultLogger((f), (#f), __func__, __FILE__, __LINE__)

#define SUCCESS(f) auto _result = _RESULT_LOGGER(f); _result
#define FAIL(f) auto _result = _RESULT_LOGGER(f); !_result

#define CHECK(f) _RESULT_LOGGER(f)
#define DISCARD(f) (void)CHECK(f)

enum NVSDK_NGX_Result;
enum NVSDK_NGX_Feature_Support_Result;
enum NVSDK_NGX_PerfQuality_Value;

const char* ToString(HRESULT value);
const char* ToString(NVSDK_NGX_Result value);
const char* ToString(NVSDK_NGX_Feature_Support_Result value);
const char* ToString(NVSDK_NGX_PerfQuality_Value value);
const char* ToString(MH_STATUS value);
const char* ToString(REFIID riid);

const char* FromWideTemp(const wchar_t* str);
std::string FromWide(const wchar_t* str);
IID IIDFromString(const char* str);

const wchar_t* TempPath();

enum class LogLevel {
    Error,
    Warning,
    Info,
    Debug,
    DebugVerbose,
};

template <typename... Args>
void LOG(LogLevel logLevel, fmt::format_string<Args...> fmt, Args&&... args)
{
#ifndef _DEBUG
    if (logLevel >= LogLevel::Debug) {
        return;
    }
    #endif

    const char* severity = nullptr;

    if (logLevel == LogLevel::Error) {
        severity = "[ERROR]";
    } else if (logLevel == LogLevel::Warning) {
        severity = "[WARNING]";
    } else if (logLevel == LogLevel::Info) {
        severity = "[INFO]";
    } else if (logLevel == LogLevel::Debug) {
        severity = "[DEBUG]";
    } else if (logLevel == LogLevel::DebugVerbose) {
        severity = "[DEBUGV]";
    }

    const char* colApply = "\033[0m";
    const char* colRevert = "\033[0m";

    if (logLevel == LogLevel::Error) {
        colApply = "\033[31m";
    } else if (logLevel == LogLevel::Warning) {
        colApply = "\033[33m";
    } else if (logLevel == LogLevel::Info) {
        colApply = "\033[36m";
    } else if (logLevel == LogLevel::Debug || logLevel == LogLevel::DebugVerbose) {
        colApply = "\033[90m";
    }

    std::string formatted = fmt::format("{} {}", severity, fmt::format(fmt, std::forward<Args>(args)...));
    std::printf("%s%s%s", colApply, formatted.c_str(), colRevert);
    OutputDebugStringA(formatted.c_str());
}

template <typename... Args>
inline void LOG_ERROR(fmt::format_string<Args...> fmt, Args&&... args)
{
    LOG(LogLevel::Error, fmt, std::forward<Args>(args)...);

    if (IsDebuggerPresent()) {
        __debugbreak();
    }
}

template <typename... Args>
inline void LOG_WARNING(fmt::format_string<Args...> fmt, Args&&... args)
{
    LOG(LogLevel::Warning, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void LOG_INFO(fmt::format_string<Args...> fmt, Args&&... args)
{
    LOG(LogLevel::Info, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void LOG_DEBUG(fmt::format_string<Args...> fmt, Args&&... args)
{
    LOG(LogLevel::Debug, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void LOG_DEBUGV(fmt::format_string<Args...> fmt, Args&&... args)
{
    LOG(LogLevel::DebugVerbose, fmt, std::forward<Args>(args)...);
}

template <typename T>
constexpr bool ResultTraits<T>::IsSuccess(T result)
{
    if constexpr (std::is_same_v<T, HRESULT>) {
        return SUCCEEDED(result);
    } else if constexpr (std::is_same_v<T, HANDLE>) {
        return result != INVALID_HANDLE_VALUE;
    } else if constexpr (std::is_same_v<T, NVSDK_NGX_Result>) {
        return NVSDK_NGX_SUCCEED(result);
    } else if constexpr (std::is_same_v<T, NVSDK_NGX_Feature_Support_Result>) {
        return result == 0;
    } else if constexpr (std::is_same_v<T, MH_STATUS>) {
        return result == 0;
    } else if constexpr (std::is_same_v<T, BOOL>) {
        return result != FALSE;
    } else {
        return result;
    }
}

template <typename T>
constexpr const char* ResultTraits<T>::AsString(T result)
{
    if constexpr (std::is_same_v<T, HANDLE> || std::is_same_v<T, BOOL>) {
        return ToString((HRESULT)GetLastError());
    } else {
        return ToString(result);
    }
}

template <typename T>
ResultLogger<T>::ResultLogger(T result, const char* expr, const char* func, const char* file, int line)
    : _result(result)
{
    static const char* _lastExpr;
    static bool _lastExprWarned = false;
    
    const bool success = bool(*this);

    if (success && _lastExpr == expr) { 
        if (_lastExprWarned) {
            LOG_DEBUGV(" (repeated)");
            _lastExprWarned = true;
        }

        return;
    }

    LOG(success ? LogLevel::DebugVerbose : LogLevel::Error,
        "<{}:{} {}>\n\tOperation: {}\n\tResult: {}\n", 
        file, line, func, expr, ResultTraits<T>::AsString(result));

    _lastExpr = expr;
    _lastExprWarned = false;
   
#if defined(_DEBUG)
    if (!success && IsDebuggerPresent()) {
        __debugbreak();
    }
#endif
}

template <typename T>
ResultLogger<T>::operator bool() const noexcept
{
    return ResultTraits<T>::IsSuccess(_result);
}

template <typename T>
ResultLogger<T>::operator T() const noexcept
{
    return _result;
}
