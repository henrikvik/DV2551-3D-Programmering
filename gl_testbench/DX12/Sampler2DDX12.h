#pragma once
#include "../Sampler2D.h"

class Sampler2DDX12 : public Sampler2D
{
public:
    // Inherited via Sampler2D
    virtual void setMagFilter(FILTER filter) override;
    virtual void setMinFilter(FILTER filter) override;
    virtual void setWrap(WRAPPING s, WRAPPING t) override;

private:

};