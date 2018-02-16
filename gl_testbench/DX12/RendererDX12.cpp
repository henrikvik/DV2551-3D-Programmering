#include "RendererDX12.h"
#include "MaterialDX12.h"
#include "RenderStateDX12.h"
#include "Texture2DDX12.h"
#include "Sampler2DDX12.h"
#include "MeshDX12.h"
#include "VertexBufferDX12.h"
#include "TechniqueDX12.h"
#include "ConstantBufferDX12.h"

#include "Helper.h"
#include <SDL_syswm.h>

#pragma comment(lib, "d3d12.lib")

RendererDX12::RendererDX12()
{
    // Create Device.
    auto adapter = findAdapter();    
    D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));
    SafeRelease(adapter);

    // Make device global.
    g.device = device;

    // Create cmd objects.
    D3D12_COMMAND_QUEUE_DESC cqd = {};
    device->CreateCommandQueue(&cqd, IID_PPV_ARGS(&cmdQueue));
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
    device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        cmdAlloc,
        nullptr,
        IID_PPV_ARGS(&cmdList)
    );
    cmdList->Close();

    // Make cmd list global.
    g.cmdList = cmdList;

    // Create fence.
    fenceValue = 0;
    fenceHandle = CreateEvent(0, false, false, 0);
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

    // Create root sign.
    createRootSign();
    // Make root sign global.
    g.rootSign = rootSign;
}

RendererDX12::~RendererDX12()
{
    SafeRelease(device);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Material * RendererDX12::makeMaterial(const std::string & name)
{
    return new MaterialDX12();
}

Mesh * RendererDX12::makeMesh()
{
    return new MeshDX12();
}

VertexBuffer * RendererDX12::makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage)
{
    return new VertexBufferDX12();
}

Texture2D * RendererDX12::makeTexture2D()
{
    return new Texture2DDX12();
}

Sampler2D * RendererDX12::makeSampler2D()
{
    return new Sampler2DDX12();
}

RenderState * RendererDX12::makeRenderState()
{
    return new RenderStateDX12();
}

std::string RendererDX12::getShaderPath()
{
    return "../assets/DX12/";
}

std::string RendererDX12::getShaderExtension()
{
    return ".hlsl";
}

ConstantBuffer * RendererDX12::makeConstantBuffer(std::string NAME, unsigned int location)
{
    return new ConstantBufferDX12(location);
}

Technique * RendererDX12::makeTechnique(Material * material, RenderState * renderState)
{
    return new TechniqueDX12(material, renderState);
}

int RendererDX12::initialize(unsigned int width, unsigned int height)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) throw "Failed to init SDL";

    window = SDL_CreateWindow(
        "DX12", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    SDL_GetWindowWMInfo(window, &wminfo);
    
    createSwapChain(width, height, wminfo.info.win.window);

    return 0;
}

void RendererDX12::setWinTitle(const char * title)
{
    SDL_SetWindowTitle(window, title);
}

void RendererDX12::present()
{
}

int RendererDX12::shutdown()
{
    return 0;
}

void RendererDX12::setClearColor(float, float, float, float)
{
}

void RendererDX12::clearBuffer(unsigned int)
{
}

void RendererDX12::setRenderState(RenderState * ps)
{
}

void RendererDX12::submit(Mesh * mesh)
{
}

void RendererDX12::frame()
{
}

void RendererDX12::waitForGPU()
{
    const UINT currentFenceValue = fenceValue;
    cmdQueue->Signal(fence, currentFenceValue);
    fenceValue++;

    if (fence->GetCompletedValue() < currentFenceValue)
    {
        fence->SetEventOnCompletion(currentFenceValue, fenceHandle);
        WaitForSingleObject(fenceHandle, INFINITE);
    }
}

IDXGIAdapter1 * RendererDX12::findAdapter()
{
    IDXGIFactory5 * factory = nullptr;
    IDXGIAdapter1 * adapter = nullptr;

    CreateDXGIFactory(IID_PPV_ARGS(&factory));
    UINT adapterIndex = 0;
    while (
        adapter == nullptr && 
        SUCCEEDED(factory->EnumAdapters1(adapterIndex++, &adapter))
    ){
        if FAILED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr))
        {
            SafeRelease(adapter);
        }
    }

    return adapter;
}

void RendererDX12::createSwapChain(UINT width, UINT height, HWND hwnd)
{
    // Create SwapChain
    IDXGIFactory5 * factory = nullptr;
    CreateDXGIFactory(IID_PPV_ARGS(&factory));

    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width            = width;
    scd.Height           = height;
    scd.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.Scaling          = DXGI_SCALING_NONE;
    scd.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount      = SWAP_BUFFER_COUNT;
    scd.SampleDesc.Count = 1;

    BreakOnFail(factory->CreateSwapChainForHwnd(
        cmdQueue,
        hwnd,
        &scd,
        nullptr,
        nullptr,
        &swapChain
    ));


    SafeRelease(factory);

    // Create Render Targets
    D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
    dhd.NumDescriptors = SWAP_BUFFER_COUNT;
    dhd.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    device->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&renderTargetHeap));

    renderTargetDescSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < SWAP_BUFFER_COUNT; i++)
    {
        swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
        device->CreateRenderTargetView(renderTargets[i], nullptr, cdh);
        cdh.ptr += renderTargetDescSize;
    }

    // Set Viewport and SciccorRect
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width    = width;
    viewport.Height   = height;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;

    scissorRect.left   = 0;
    scissorRect.top    = 0;
    scissorRect.right  = width;
    scissorRect.bottom = height;
}

void RendererDX12::createRootSign()
{
    auto makeDescRange = [](D3D12_DESCRIPTOR_RANGE_TYPE drt, UINT base, UINT num)
    {
        D3D12_DESCRIPTOR_RANGE dr;
        dr.RangeType = drt;
        dr.BaseShaderRegister = base;
        dr.NumDescriptors = num;
        dr.RegisterSpace = 0;
        dr.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        return dr;
    };

    auto makeRootDescTable = [](D3D12_DESCRIPTOR_RANGE ranges[], UINT num)
    {
        D3D12_ROOT_DESCRIPTOR_TABLE dt;
        dt.pDescriptorRanges = ranges;
        dt.NumDescriptorRanges = num;
        return dt;
    };

    auto makeRootParam = [](D3D12_ROOT_DESCRIPTOR_TABLE table, D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER rp;
        rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rp.ShaderVisibility = visibility;
        rp.DescriptorTable = table;
        return rp;
    };

    D3D12_DESCRIPTOR_RANGE dr_vertex[] = {
        makeDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 3)
    };
    D3D12_DESCRIPTOR_RANGE dr_pixel[] = {
        makeDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,     0, 1)
    };
    D3D12_DESCRIPTOR_RANGE dr_all[] = {
        makeDescRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 5, 2)
    };

    D3D12_ROOT_DESCRIPTOR_TABLE rdt_vertex = makeRootDescTable(dr_vertex, ARRAYSIZE(dr_vertex));
    D3D12_ROOT_DESCRIPTOR_TABLE rdt_pixel  = makeRootDescTable(dr_pixel,  ARRAYSIZE(dr_pixel));
    D3D12_ROOT_DESCRIPTOR_TABLE rdt_all    = makeRootDescTable(dr_all,    ARRAYSIZE(dr_all));

    D3D12_ROOT_PARAMETER rps[] = {
        makeRootParam(rdt_vertex, D3D12_SHADER_VISIBILITY_VERTEX),
        makeRootParam(rdt_pixel,  D3D12_SHADER_VISIBILITY_PIXEL),
        makeRootParam(rdt_all,    D3D12_SHADER_VISIBILITY_ALL),
    };

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC vrsd;
    vrsd.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
    vrsd.Desc_1_0.NumParameters = ARRAYSIZE(rps);
    vrsd.Desc_1_0.pParameters = rps;
    vrsd.Desc_1_0.NumStaticSamplers = 0;
    vrsd.Desc_1_0.pStaticSamplers = nullptr;
    vrsd.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

    ID3DBlob * blob, * error;
    BreakOnFail2(D3D12SerializeVersionedRootSignature(
        &vrsd,
        &blob,
        &error
    ), (char*)error->GetBufferPointer());

    BreakOnFail(device->CreateRootSignature(
        0,
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        IID_PPV_ARGS(&rootSign)
    ));

    SafeRelease(blob);
}
