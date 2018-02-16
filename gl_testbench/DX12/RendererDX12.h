#pragma once
#include "../Renderer.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <SDL.h>


#pragma comment(lib, "dxgi.lib")

#define SWAP_BUFFER_COUNT 2

class RendererDX12 : public Renderer
{
public:
    RendererDX12();
    virtual ~RendererDX12();

    // Inherited via Renderer
    virtual Material * makeMaterial(const std::string & name) override;
    virtual Mesh * makeMesh() override;
    virtual VertexBuffer * makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage) override;
    virtual Texture2D * makeTexture2D() override;
    virtual Sampler2D * makeSampler2D() override;
    virtual RenderState * makeRenderState() override;
    virtual std::string getShaderPath() override;
    virtual std::string getShaderExtension() override;
    virtual ConstantBuffer * makeConstantBuffer(std::string NAME, unsigned int location) override;
    virtual Technique * makeTechnique(Material *, RenderState *) override;
    virtual int initialize(unsigned int width = 800, unsigned int height = 600) override;
    virtual void setWinTitle(const char * title) override;
    virtual void present() override;
    virtual int shutdown() override;
    virtual void setClearColor(float, float, float, float) override;
    virtual void clearBuffer(unsigned int) override;
    virtual void setRenderState(RenderState * ps) override;
    virtual void submit(Mesh * mesh) override;
    virtual void frame() override;

    void waitForGPU();

private:
    IDXGIAdapter1 * findAdapter();
    void createSwapChain(UINT width, UINT height, HWND hwnd);
    void createRootSign();


    ID3D12Device * device;

    ID3D12CommandQueue * cmdQueue;
    ID3D12CommandAllocator * cmdAlloc;
    ID3D12GraphicsCommandList * cmdList;

    UINT fenceValue;
    HANDLE fenceHandle;
    ID3D12Fence * fence;

    IDXGISwapChain1 * swapChain;

    SDL_Window * window;

    ID3D12DescriptorHeap * renderTargetHeap;
    ID3D12Resource * renderTargets[SWAP_BUFFER_COUNT];
    UINT renderTargetDescSize;

    D3D12_VIEWPORT viewport;
    D3D12_RECT scissorRect;

    ID3D12RootSignature * rootSign;
    
};