#include <comdef.h>
#include <nvsdk_ngx.h>
#include <filesystem>

#include <Util.hpp>

const char* ToString(HRESULT value)
{
    return FromWideTemp(_com_error(value).ErrorMessage());
}

const char* ToString(NVSDK_NGX_Result value)
{
    switch (value) {
    case NVSDK_NGX_Result_Success:
        return "NVSDK_NGX_Result_Success";
    case NVSDK_NGX_Result_Fail:
        return "NVSDK_NGX_Result_Fail";
    case NVSDK_NGX_Result_FAIL_FeatureNotSupported:
        return "NVSDK_NGX_Result_FAIL_FeatureNotSupported";
    case NVSDK_NGX_Result_FAIL_PlatformError:
        return "NVSDK_NGX_Result_FAIL_PlatformError";
    case NVSDK_NGX_Result_FAIL_FeatureAlreadyExists:
        return "NVSDK_NGX_Result_FAIL_FeatureAlreadyExists";
    case NVSDK_NGX_Result_FAIL_FeatureNotFound:
        return "NVSDK_NGX_Result_FAIL_FeatureNotFound";
    case NVSDK_NGX_Result_FAIL_InvalidParameter:
        return "NVSDK_NGX_Result_FAIL_InvalidParameter";
    case NVSDK_NGX_Result_FAIL_ScratchBufferTooSmall:
        return "NVSDK_NGX_Result_FAIL_ScratchBufferTooSmall";
    case NVSDK_NGX_Result_FAIL_NotInitialized:
        return "NVSDK_NGX_Result_FAIL_NotInitialized";
    case NVSDK_NGX_Result_FAIL_UnsupportedInputFormat:
        return "NVSDK_NGX_Result_FAIL_UnsupportedInputFormat";
    case NVSDK_NGX_Result_FAIL_RWFlagMissing:
        return "NVSDK_NGX_Result_FAIL_RWFlagMissing";
    case NVSDK_NGX_Result_FAIL_MissingInput:
        return "NVSDK_NGX_Result_FAIL_MissingInput";
    case NVSDK_NGX_Result_FAIL_UnableToInitializeFeature:
        return "NVSDK_NGX_Result_FAIL_UnableToInitializeFeature";
    case NVSDK_NGX_Result_FAIL_OutOfDate:
        return "NVSDK_NGX_Result_FAIL_OutOfDate";
    case NVSDK_NGX_Result_FAIL_OutOfGPUMemory:
        return "NVSDK_NGX_Result_FAIL_OutOfGPUMemory";
    case NVSDK_NGX_Result_FAIL_UnsupportedFormat:
        return "NVSDK_NGX_Result_FAIL_UnsupportedFormat";
    case NVSDK_NGX_Result_FAIL_UnableToWriteToAppDataPath:
        return "NVSDK_NGX_Result_FAIL_UnableToWriteToAppDataPath";
    case NVSDK_NGX_Result_FAIL_UnsupportedParameter:
        return "NVSDK_NGX_Result_FAIL_UnsupportedParameter";
    case NVSDK_NGX_Result_FAIL_Denied:
        return "NVSDK_NGX_Result_FAIL_Denied";
    case NVSDK_NGX_Result_FAIL_NotImplemented:
        return "NVSDK_NGX_Result_FAIL_NotImplemented";
    default:
        return "(unknown)";
    }
}

const char* ToString(NVSDK_NGX_Feature_Support_Result value)
{
    switch (value) {
    case NVSDK_NGX_FeatureSupportResult_Supported:
        return "NVSDK_NGX_FeatureSupportResult_Supported";
    case NVSDK_NGX_FeatureSupportResult_CheckNotPresent:
        return "NVSDK_NGX_FeatureSupportResult_CheckNotPresent";
    case NVSDK_NGX_FeatureSupportResult_DriverVersionUnsupported:
        return "NVSDK_NGX_FeatureSupportResult_DriverVersionUnsupported";
    case NVSDK_NGX_FeatureSupportResult_AdapterUnsupported:
        return "NVSDK_NGX_FeatureSupportResult_AdapterUnsupported";
    case NVSDK_NGX_FeatureSupportResult_OSVersionBelowMinimumSupported:
        return "NVSDK_NGX_FeatureSupportResult_OSVersionBelowMinimumSupported";
    case NVSDK_NGX_FeatureSupportResult_NotImplemented:
        return "NVSDK_NGX_FeatureSupportResult_NotImplemented";
    default:
        return "(unknown)";
    }
}

const char* ToString(NVSDK_NGX_PerfQuality_Value value)
{
    switch (value) {
    case NVSDK_NGX_PerfQuality_Value_MaxPerf:
        return "NVSDK_NGX_PerfQuality_Value_MaxPerf";
    case NVSDK_NGX_PerfQuality_Value_Balanced:
        return "NVSDK_NGX_PerfQuality_Value_Balanced";
    case NVSDK_NGX_PerfQuality_Value_MaxQuality:
        return "NVSDK_NGX_PerfQuality_Value_MaxQuality";
    case NVSDK_NGX_PerfQuality_Value_UltraPerformance:
        return "NVSDK_NGX_PerfQuality_Value_UltraPerformance";
    case NVSDK_NGX_PerfQuality_Value_UltraQuality:
        return "NVSDK_NGX_PerfQuality_Value_UltraQuality";
    case NVSDK_NGX_PerfQuality_Value_DLAA:
        return "NVSDK_NGX_PerfQuality_Value_DLAA";
    default:
        return "(unknown)";
    }
}

const char* ToString(MH_STATUS value)
{
    return MH_StatusToString(value);
}

const char* ToString(REFIID riid)
{
    LPOLESTR str = nullptr;
    DISCARD(StringFromIID(riid, &str));
    return FromWideTemp(str);
}

const char* FromWideTemp(const wchar_t* str)
{
    static char conv[512];
    assert(sizeof(conv) >= MB_CUR_MAX + 1);

    size_t convertedChars = 0;
    wcstombs_s(&convertedChars, conv, str, sizeof(conv));

    return conv;
}

std::string FromWide(const wchar_t* str)
{
    return FromWideTemp(str);
}

IID IIDFromString(const char* str)
{
    IID iid;
    DISCARD(IIDFromString(_bstr_t(str), &iid));
    return iid;
}

const wchar_t* TempPath()
{
    static std::wstring _tempPath = std::filesystem::temp_directory_path().wstring();
    return _tempPath.c_str();
}
