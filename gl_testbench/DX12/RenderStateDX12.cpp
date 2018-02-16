#include "RenderStateDX12.h"

RenderStateDX12::RenderStateDX12()
{
    wireFrame = false;
}

RenderStateDX12::~RenderStateDX12()
{
}

void RenderStateDX12::setWireFrame(bool wf)
{
    wireFrame = wf;
}

void RenderStateDX12::set()
{
}

bool RenderStateDX12::getWireFrame() const 
{
    return wireFrame; 
}
