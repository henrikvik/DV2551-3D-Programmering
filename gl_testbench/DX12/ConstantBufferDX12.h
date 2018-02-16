#pragma once
#include "../ConstantBuffer.h"
#include <d3d12.h>

class ConstantBufferDX12 : public ConstantBuffer
{
public:
    ConstantBufferDX12(UINT location);
    virtual ~ConstantBufferDX12();
    // Inherited via ConstantBuffer
    virtual void setData(const void * data, size_t size, Material * m = nullptr, unsigned int location = 0) override;
    virtual void bind(Material * = nullptr) override;

private:
    UINT location;
    ID3D12Resource * constantBuffer;
    void * dataPtr;

    void create(UINT size);
};