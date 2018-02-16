#pragma once
#include <comdef.h>
#include <d3d12.h>

#define SafeRelease(ptr) {if (ptr) ptr->Release(); ptr = nullptr;}
#define BreakOnFail(hr)  {\
    if FAILED(hr) \
    { \
        OutputDebugString(_bstr_t("\n---==+==---\n"));\
        OutputDebugString(_com_error(hr).ErrorMessage());\
        OutputDebugString(_bstr_t("\n---==+==---\n"));\
        _CrtDbgBreak();\
    }\
}

#define BreakOnFail2(hr, message)\
{\
    if FAILED(hr)\
    {\
        OutputDebugString(_bstr_t("\n---==+==---\n"));\
        OutputDebugString(_bstr_t(message));\
        OutputDebugString(_bstr_t("\n---==+==---\n"));\
        _CrtDbgBreak();\
    }\
}

struct g_t
{
    ID3D12Device * device;
    ID3D12GraphicsCommandList * cmdList;
    ID3D12RootSignature * rootSign;
};

extern g_t g;