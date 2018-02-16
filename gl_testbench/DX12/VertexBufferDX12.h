#pragma once
#include "../VertexBuffer.h"

class VertexBufferDX12 : public VertexBuffer
{
public:

    // Inherited via VertexBuffer
    virtual void setData(const void * data, size_t size, size_t offset) override;
    virtual void bind(size_t offset, size_t size, unsigned int location) override;
    virtual void unbind() override;
    virtual size_t getSize() override;
};