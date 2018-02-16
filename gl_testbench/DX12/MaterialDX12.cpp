#include "MaterialDX12.h"
#include "Helper.h"
#include <d3dcompiler.h>
#include <fstream>

#pragma comment(lib, "d3dcompiler.lib")

MaterialDX12::MaterialDX12()
{
}

MaterialDX12::~MaterialDX12()
{
    for (auto & type_fileName : shaderFileNames)
    {
        removeShader(type_fileName.first);
    }

    for (auto & location_constantBuffer : constantBuffers)
    {
        delete location_constantBuffer.second;
    }
}

void MaterialDX12::setShader(const std::string & shaderFileName, ShaderType type)
{
    removeShader(type);
    shaderFileNames[type] = shaderFileName;
}

void MaterialDX12::removeShader(ShaderType type)
{
    // Remove File Name
    {
        auto it = shaderFileNames.find(type);
        if (it != shaderFileNames.end()) shaderFileNames.erase(it);
    }

    // Remove Blob
    {
        auto it = blobs.find(type);
        if (it != blobs.end()) 
        {
            SafeRelease(it->second);
            blobs.erase(it);
        }
    }
}

void MaterialDX12::setDiffuse(Color c)
{
}

int MaterialDX12::compileMaterial(std::string & errString)
{
    static std::map<ShaderType, std::pair<const char*, const char*>> shaderSettings = {
        { ShaderType::VS, { "VSMain", "vs_5_1" }},
        { ShaderType::PS, { "PSMain", "ps_5_1" }},
    };

    for (auto type_fileName : shaderFileNames)
    {
        auto type = type_fileName.first;
        auto fileName = type_fileName.second;

        ID3DBlob* blob, * error;

        std::ifstream fin(fileName);
        std::string shaderCode((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        fin.close();

        std::string defines;
        for (auto define : shaderDefines[type])
        {
            defines += define;
        }
        
        shaderCode = defines + shaderCode;

        std::ofstream fout(fileName + "_defines.hlsl");
        fout << shaderCode;
        fout.close();


        UINT compileFlags = 0;
        #ifdef _DEBUG
        compileFlags += D3DCOMPILE_DEBUG;
        compileFlags += D3DCOMPILE_SKIP_OPTIMIZATION;
        #endif

        BreakOnFail2(D3DCompile(
            shaderCode.c_str(),
            shaderCode.size(),
            fileName.c_str(),
            nullptr,
            nullptr,
            shaderSettings[type].first,
            shaderSettings[type].second,
            compileFlags,
            0,
            &blob,
            &error
        ), (char*)error->GetBufferPointer());

        blobs[type] = blob;
    }

    return 0;
}

void MaterialDX12::addConstantBuffer(std::string name, unsigned int location)
{
    if (constantBuffers[location] != nullptr)
    {
        delete constantBuffers[location];
    }

    constantBuffers[location] = new ConstantBufferDX12(location);
}

void MaterialDX12::updateConstantBuffer(const void * data, size_t size, unsigned int location)
{
    constantBuffers[location]->setData(data, size);
}

int MaterialDX12::enable()
{
    return 0;
}

void MaterialDX12::disable()
{
}

ID3DBlob * MaterialDX12::getBlob(ShaderType type)
{
    return blobs[type];
}
