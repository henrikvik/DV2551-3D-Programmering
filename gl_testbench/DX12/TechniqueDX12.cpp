#include "TechniqueDX12.h"
#include "Helper.h"
#include "MaterialDX12.h"
#include "RenderStateDX12.h"
#include "d3dx12.h"

TechniqueDX12::TechniqueDX12(Material * material, RenderState * renderState)
    : Technique(material, renderState)
{
    auto mat = dynamic_cast<MaterialDX12*>(material);
    auto vertexBlob = mat->getBlob(Material::ShaderType::VS);
    auto pixelBlob  = mat->getBlob(Material::ShaderType::PS);

    CD3DX12_RASTERIZER_DESC rasterizer(D3D12_DEFAULT);
    rasterizer.FillMode = dynamic_cast<RenderStateDX12*>(renderState)->getWireFrame() ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};
    gpsd.RasterizerState       = rasterizer;
    gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    gpsd.BlendState       = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    gpsd.SampleMask       = UINT_MAX;
    gpsd.SampleDesc.Count = 1;

    gpsd.NumRenderTargets = 1;
    gpsd.RTVFormats[0]    = DXGI_FORMAT_R8G8B8A8_UNORM;

    gpsd.DepthStencilState.DepthEnable      = FALSE;
    gpsd.DepthStencilState.StencilEnable    = FALSE;

    gpsd.pRootSignature = g.rootSign;
    gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexBlob);
    gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelBlob);

    BreakOnFail(g.device->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(&pipelineState)));
}

TechniqueDX12::~TechniqueDX12()
{
    SafeRelease(pipelineState);
}

ID3D12PipelineState * TechniqueDX12::getPipelineState()
{
    return pipelineState;
}
