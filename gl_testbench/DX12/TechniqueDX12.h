#pragma once
#include "../Technique.h"
#include <d3d12.h>

class TechniqueDX12 : public Technique
{
public:
    TechniqueDX12(Material * material, RenderState * renderState);
    virtual ~TechniqueDX12();

    ID3D12PipelineState * getPipelineState();

private:
    ID3D12PipelineState * pipelineState;
};