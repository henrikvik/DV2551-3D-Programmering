#pragma once
#include "../RenderState.h"

class RenderStateDX12 : public RenderState
{
public:
    RenderStateDX12();
    virtual ~RenderStateDX12();

    // Inherited via RenderState
    virtual void setWireFrame(bool) override;
    virtual void set() override;

    bool getWireFrame() const;

private:
    bool wireFrame;
};