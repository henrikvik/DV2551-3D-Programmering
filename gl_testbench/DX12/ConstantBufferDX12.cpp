#include "ConstantBufferDX12.h"
#include "Helper.h"
#include "d3dx12.h"

ConstantBufferDX12::ConstantBufferDX12(UINT location)
{
    this->location = location;
    constantBuffer = nullptr;
    dataPtr = nullptr;
}

ConstantBufferDX12::~ConstantBufferDX12()
{
    SafeRelease(constantBuffer);
}

void ConstantBufferDX12::setData(const void * data, size_t size, Material * m, unsigned int location)
{
    if (constantBuffer == nullptr) create(size);
    memcpy(dataPtr, data, size);
}

void ConstantBufferDX12::bind(Material *)
{
    g.cmdList->SetGraphicsRootConstantBufferView(location, constantBuffer->GetGPUVirtualAddress());
}

void ConstantBufferDX12::create(UINT size)
{
    BreakOnFail(g.device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer((size + 255) & ~255),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer)
    ));

    CD3DX12_RANGE readRange(0,0);
    BreakOnFail(constantBuffer->Map(0, &readRange, &dataPtr));
}
