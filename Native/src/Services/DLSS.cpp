#include <d3d11.h>
#include <nvsdk_ngx.h>
#include <nvsdk_ngx_helpers.h>

#include <Services.hpp>
#include <Services/DLSS.hpp>
#include <Services/Debug/Inspector.hpp>
#include <Services/Platform/D3D11.hpp>
#include <Services/Platform/DXGI.hpp>
#include <Util.hpp>

#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

#define DLSS_RUN_TESTS 0

using namespace DLSS;

struct {
    bool Initialized;
    std::mutex Lock;
    NVSDK_NGX_Handle* Handle;
    NVSDK_NGX_Parameter* Params;
    QualityMode Mode;
    EvaluationFlags Flags; 
    std::unique_ptr<NVSDK_NGX_D3D11_DLSS_Eval_Params> PendingEvalParams;
} _state;

static void Initialize(ID3D11Device* device, ID3D11DeviceContext* immediateContext);
static int GetQualityModes_Impl(uint32_t finalWidth, uint32_t finalHeight, QualityMode* outQualityModes);
static void SetQualityMode_Impl(const QualityMode* mode, const EvaluationFlags flags);
static void Evaluate_Impl(RenderResource colorIn, RenderResource colorOut, float sharpness, const EvaluationParams* params);

int WINAPI GetQualityModes(uint32_t finalWidth, uint32_t finalHeight, QualityMode* outQualityModes)
{
    std::lock_guard _(_state.Lock);
    Initialize(D3D11::GetDevice(), D3D11::GetImmediateContext());
    return GetQualityModes_Impl(finalWidth, finalHeight, outQualityModes);
}

void WINAPI SetQualityMode(const QualityMode* mode, const EvaluationFlags flags)
{
    std::lock_guard _(_state.Lock);
    Initialize(D3D11::GetDevice(), D3D11::GetImmediateContext());
    SetQualityMode_Impl(mode, flags);
}

void WINAPI Evaluate(RenderResource colorIn, RenderResource colorOut, float sharpness, const EvaluationParams* params)
{
    std::lock_guard _(_state.Lock);
    Initialize(D3D11::GetDevice(), D3D11::GetImmediateContext());
    Evaluate_Impl(colorIn, colorOut, sharpness, params);
}

static int GetQualityModes_Impl(uint32_t finalWidth, uint32_t finalHeight, QualityMode* outQualityModes)
{
    assert(_state.Initialized);

    NVSDK_NGX_Parameter* params = nullptr;

    if (FAIL(NVSDK_NGX_D3D11_AllocateParameters(&params))) {
        return 0;
    }

    int count = 0;

    if (SUCCEEDED(NVSDK_NGX_D3D11_GetCapabilityParameters(&params))) {
        NVSDK_NGX_PerfQuality_Value preset = NVSDK_NGX_PerfQuality_Value_MaxPerf;

        while (preset <= NVSDK_NGX_PerfQuality_Value_DLAA) {
            uint32_t width = 0, dynWidthMin = 0, dynWidthMax = 0;
            uint32_t height = 0, dynHeightMin = 0, dynHeightMax = 0;
            float sharpness = 0.0f;

            NVSDK_NGX_PerfQuality_Value curPreset = preset;
            preset = (NVSDK_NGX_PerfQuality_Value)(curPreset + 1);

            if (FAIL(NGX_DLSS_GET_OPTIMAL_SETTINGS(
                    params,
                    finalWidth, finalHeight,
                    curPreset,
                    &width, &height,
                    &dynWidthMax, &dynHeightMax,
                    &dynWidthMin, &dynHeightMin,
                    &sharpness))) {
                continue;
            }

            if (width == 0 || height == 0) {
                continue;
            }

            ++count;

            if (outQualityModes) {
                outQualityModes->Name = ToString(curPreset);
                outQualityModes->InputWidth = width;
                outQualityModes->InputHeight = height;
                outQualityModes->FinalWidth = finalWidth;
                outQualityModes->FinalHeight = finalHeight;
                ++outQualityModes;
            }
        }
    }

    DISCARD(NVSDK_NGX_D3D11_DestroyParameters(params));
    return count;
}

static void SetQualityMode_Impl(const QualityMode* mode, const EvaluationFlags flags)
{
    assert(_state.Initialized);

    if (_state.Handle) {
        if (_state.Mode.InputWidth == mode->InputWidth &&
            _state.Mode.InputHeight == mode->InputHeight &&
            _state.Mode.FinalWidth == mode->FinalWidth &&
            _state.Mode.FinalHeight == mode->FinalHeight &&
            _state.Flags == flags
        ) {
            return; // Already selected, no need to recreate.
        }

        DISCARD(NVSDK_NGX_D3D11_ReleaseFeature(_state.Handle));
        DISCARD(NVSDK_NGX_D3D11_DestroyParameters(_state.Params));

        _state.Handle = nullptr;
        _state.Params = nullptr;
    }

    assert(!_state.Handle);
    assert(!_state.Params);

    NVSDK_NGX_Parameter* params = nullptr;

    if (FAIL(NVSDK_NGX_D3D11_AllocateParameters(&params))) {
        return;
    }

    static std::unordered_map<const char*, NVSDK_NGX_PerfQuality_Value> _qualityMap = {
        { ToString(NVSDK_NGX_PerfQuality_Value_MaxPerf), NVSDK_NGX_PerfQuality_Value_MaxPerf },
        { ToString(NVSDK_NGX_PerfQuality_Value_Balanced), NVSDK_NGX_PerfQuality_Value_Balanced },
        { ToString(NVSDK_NGX_PerfQuality_Value_MaxQuality), NVSDK_NGX_PerfQuality_Value_MaxQuality },
        { ToString(NVSDK_NGX_PerfQuality_Value_UltraPerformance), NVSDK_NGX_PerfQuality_Value_UltraPerformance },
        { ToString(NVSDK_NGX_PerfQuality_Value_UltraQuality), NVSDK_NGX_PerfQuality_Value_UltraQuality },
        { ToString(NVSDK_NGX_PerfQuality_Value_DLAA), NVSDK_NGX_PerfQuality_Value_DLAA }
    };

    NVSDK_NGX_DLSS_Create_Params createParams = {
        .Feature = NVSDK_NGX_Feature_Create_Params {
            .InWidth = mode->InputWidth,
            .InHeight = mode->InputHeight,
            .InTargetWidth = mode->FinalWidth,
            .InTargetHeight = mode->FinalHeight,
            .InPerfQualityValue = _qualityMap[mode->Name],
        },
        .InFeatureCreateFlags = flags
    };

    NVSDK_NGX_Handle* handle = nullptr;

    if (FAIL(NGX_D3D11_CREATE_DLSS_EXT(D3D11::GetImmediateContext(), &handle, params, &createParams))) {
        DISCARD(NVSDK_NGX_D3D11_DestroyParameters(params));
        return;
    }

    _state.Handle = handle;
    _state.Params = params;
    _state.Mode = *mode;
    _state.Flags = flags;
}

static void Evaluate_Impl(RenderResource colorIn, RenderResource colorOut, float sharpness, const EvaluationParams* params)
{
    assert(_state.Initialized);
    assert(_state.Handle);
    assert(_state.Params);
    assert(!_state.PendingEvalParams);

    assert(colorIn);
    assert(colorOut);

    assert(params);
    assert(params->DepthIn);
    assert(params->MvecIn);

    NVSDK_NGX_D3D11_DLSS_Eval_Params evalParams = {
        .Feature = {
            .pInColor = (ID3D11Resource*)colorIn,
            .pInOutput = (ID3D11Resource*)colorOut,
            .InSharpness = sharpness,
        },
        .pInDepth = (ID3D11Resource*)params->DepthIn,
        .pInMotionVectors = (ID3D11Resource*)params->MvecIn,
        .InJitterOffsetX = params->JitterX,
        .InJitterOffsetY = params->JitterY,
        .InRenderSubrectDimensions = { .Width = _state.Mode.InputWidth, .Height = _state.Mode.InputHeight },
        .InReset = params->Reset,
        .InMVScaleX = params->MVecScaleX,
        .InMVScaleY = params->MVecScaleY,
        .InIndicatorInvertYAxis = true
    };

    evalParams.Feature.pInColor->AddRef();
    evalParams.Feature.pInOutput->AddRef();
    evalParams.pInDepth->AddRef();
    evalParams.pInMotionVectors->AddRef();

    Inspector::PinResourceForFrame(evalParams.Feature.pInColor);
    Inspector::PinResourceForFrame(evalParams.Feature.pInOutput);
    Inspector::PinResourceForFrame(evalParams.pInDepth);
    Inspector::PinResourceForFrame(evalParams.pInMotionVectors);

    _state.PendingEvalParams = std::make_unique<NVSDK_NGX_D3D11_DLSS_Eval_Params>(evalParams);
}

static void Initialize_RunTests();

static void Initialize(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
{
    if (_state.Initialized) {
        return;
    }

    IDXGIAdapter* adapter = DXGI::GetAdapter();

    const NVSDK_NGX_FeatureCommonInfo featureCommonInfo = {
        .LoggingInfo = NVSDK_NGX_LoggingInfo {
            .LoggingCallback = [](const char* msg, NVSDK_NGX_Logging_Level level, NVSDK_NGX_Feature source) {
                LOG(level == NVSDK_NGX_LOGGING_LEVEL_VERBOSE ? LogLevel::Debug : LogLevel::Info, "NGX: {}", msg);
            },
            .DisableOtherLoggingSinks = true }
    };

    static constexpr const char* _projectId = "33edfdb6-226b-463c-8c08-5f43e8aa6b82";
    static constexpr NVSDK_NGX_EngineType _engineType = NVSDK_NGX_ENGINE_TYPE_UNITY;
    static constexpr const char* _engineVersion = "1";

    const NVSDK_NGX_FeatureDiscoveryInfo discovery = {
        .SDKVersion = NVSDK_NGX_Version_API,
        .FeatureID = NVSDK_NGX_Feature_SuperSampling,
        .Identifier = NVSDK_NGX_Application_Identifier {
            .IdentifierType = NVSDK_NGX_Application_Identifier_Type_Project_Id,
            .v = NVSDK_NGX_ProjectIdDescription {
                .ProjectId = _projectId,
                .EngineType = _engineType,
                .EngineVersion = _engineVersion } },
        .ApplicationDataPath = TempPath(),
        .FeatureInfo = &featureCommonInfo
    };

    NVSDK_NGX_FeatureRequirement requirement = {};

    if (FAIL(NVSDK_NGX_D3D11_GetFeatureRequirements(adapter, &discovery, &requirement))) {
        return;
    }

    if (FAIL(requirement.FeatureSupported)) {
        std::string errorString;

        if (requirement.FeatureSupported & NVSDK_NGX_FeatureSupportResult_Supported) {
            errorString += " NVSDK_NGX_FeatureSupportResult_Supported";
        }

        if (requirement.FeatureSupported & NVSDK_NGX_FeatureSupportResult_CheckNotPresent) {
            errorString += " NVSDK_NGX_FeatureSupportResult_CheckNotPresent";
        }

        if (requirement.FeatureSupported & NVSDK_NGX_FeatureSupportResult_DriverVersionUnsupported) {
            errorString += " NVSDK_NGX_FeatureSupportResult_DriverVersionUnsupported";
        }

        if (requirement.FeatureSupported & NVSDK_NGX_FeatureSupportResult_AdapterUnsupported) {
            errorString += " NVSDK_NGX_FeatureSupportResult_AdapterUnsupported";
        }

        if (requirement.FeatureSupported & NVSDK_NGX_FeatureSupportResult_OSVersionBelowMinimumSupported) {
            errorString += " NVSDK_NGX_FeatureSupportResult_OSVersionBelowMinimumSupported";
        }

        if (requirement.FeatureSupported & NVSDK_NGX_FeatureSupportResult_NotImplemented) {
            errorString += " NVSDK_NGX_FeatureSupportResult_NotImplemented";
        }

        LOG_ERROR("NVSDK_NGX_D3D11_GetFeatureRequirements failed: {}\n", errorString);
        return;
    }

    if (FAIL(NVSDK_NGX_D3D11_Init_with_ProjectID(_projectId, _engineType, _engineVersion, TempPath(), device, &featureCommonInfo))) {
        return;
    }

    static bool first = true;

    DXGI::On_Present_After([](IDXGISwapChain*) {
        first = true;
    });

    D3D11::On_SetRenderTargets([](const std::vector<ID3D11RenderTargetView*>& rtvs, ID3D11DepthStencilView* dsv) {
        std::lock_guard _(_state.Lock);

        ID3D11RenderTargetView* match = nullptr;
        NVSDK_NGX_D3D11_DLSS_Eval_Params* params = _state.PendingEvalParams.get();

        if (first && params) {
            for (ID3D11RenderTargetView* rtv : rtvs) {
                if (!rtv) {
                    continue;
                }

                ID3D11Resource* res = D3D11::GetResource(rtv);
                res->Release();

                if (res == params->Feature.pInOutput) {
                    match = rtv;
                    break;
                }
            }
        }

        if (match) {
            // Validate that all the resources still match (this can fail if any resizing is happening).
            std::vector<std::tuple<ID3D11Resource*, uint32_t, uint32_t>> inputs = {
                { params->Feature.pInColor, _state.Mode.InputWidth, _state.Mode.InputHeight },
                { params->Feature.pInOutput, _state.Mode.FinalWidth, _state.Mode.FinalHeight },
                { params->pInDepth, _state.Mode.InputWidth, _state.Mode.InputHeight },
                { params->pInMotionVectors, _state.Mode.InputWidth, _state.Mode.InputHeight },
            };

            bool validationFailure = false;

            for (auto& [res, width, height] : inputs) {
                D3D11_TEXTURE2D_DESC desc = {};
                if (ID3D11Texture2D * tex; SUCCEEDED(res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex))) {
                    tex->GetDesc(&desc);
                    tex->Release();
                }

                if (desc.Width != width || desc.Height != height) {
                    LOG_WARNING("DLSS input texture size mismatch: {}x{} vs {}x{} - {}\n", desc.Width, desc.Height, width, height, D3D11::GetDebugName(res));
                    validationFailure = true;
                }
            }

            if (!validationFailure) {
                DISCARD(NGX_D3D11_EVALUATE_DLSS_EXT(D3D11::GetImmediateContext(), _state.Handle, _state.Params, params));
            } else {
                const float rgba[] = { 1.0f, 0.0f, 0.0f, 1.0f };
                D3D11::GetImmediateContext()->ClearRenderTargetView(match, rgba);
            }

            params->Feature.pInColor->Release();
            params->Feature.pInOutput->Release();
            params->pInDepth->Release();
            params->pInMotionVectors->Release();

            _state.PendingEvalParams.reset();
            first = false;
        }
    });

    _state.Initialized = true;

    Initialize_RunTests();
}

static void Initialize_RunTests()
{
#if DLSS_RUN_TESTS == 0
    return;
#endif

    assert(_state.Initialized);

    int numModes = GetQualityModes(1920, 1080, nullptr);
    assert(numModes > 0);

    std::vector<QualityMode> modes(numModes);
    int numModesWithData = GetQualityModes(1920, 1080, modes.data());
    assert(numModesWithData > 0);

    SetQualityMode(&modes[0], {});
    assert(_state.Handle);
    assert(_state.Params);

    NVSDK_NGX_Handle* handle = _state.Handle;
    NVSDK_NGX_Parameter* params = _state.Params;

    SetQualityMode(&modes[0], {});
    assert(_state.Handle == handle);
    assert(_state.Params == params);

    SetQualityMode(&modes[1], {});
    assert(_state.Handle && _state.Handle != handle);
    assert(_state.Params && _state.Params != params);
}

static bool Initialize()
{
    return true;
}

static FARPROC GetHook(const char* fn)
{
    if (strcmp(fn, "DLSS_Evaluate") == 0) {
        return (FARPROC)Evaluate;
    }

    if (strcmp(fn, "DLSS_GetQualityModes") == 0) {
        return (FARPROC)GetQualityModes;
    }

    if (strcmp(fn, "DLSS_SetQualityMode") == 0) {
        return (FARPROC)SetQualityMode;
    }

    return nullptr;
}

REGISTER_SERVICE(
    .Name = "DLSS",
    .FnInitialize = Initialize,
    .FnGetHook = GetHook
);
