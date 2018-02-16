#include "OpenGL/OpenGLRenderer.h"
#include "Renderer.h"

#include "DX12\RendererDX12.h"


Renderer* Renderer::makeRenderer(BACKEND option)
{
    switch (option)
    {
        case BACKEND::GL45: return new OpenGLRenderer();
        case BACKEND::DX12: return new RendererDX12();
    }

    return nullptr;
}

