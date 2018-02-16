#pragma once
#include "../Material.h"
#include "ConstantBufferDX12.h"
#include <d3d12.h>

class MaterialDX12 : public Material
{
public:
    MaterialDX12();
    virtual ~MaterialDX12();

    // Inherited via Material
    virtual void setShader(const std::string & shaderFileName, ShaderType type) override;
    virtual void removeShader(ShaderType type) override;
    virtual void setDiffuse(Color c) override;
    virtual int compileMaterial(std::string & errString) override;
    virtual void addConstantBuffer(std::string name, unsigned int location) override;
    virtual void updateConstantBuffer(const void * data, size_t size, unsigned int location) override;
    virtual int enable() override;
    virtual void disable() override;

    ID3DBlob* getBlob(ShaderType type);

private:
    std::map<ShaderType, ID3DBlob*> blobs;
    std::map<UINT, ConstantBufferDX12*> constantBuffers;
};