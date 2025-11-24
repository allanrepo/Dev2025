#pragma once

#include "IRenderer.h"

namespace graphics::renderer
{
    class IRendererImpl: public graphics::renderer::IRenderer
    {
    public:
        // Virtual destructor for safe polymorphic cleanup
        virtual ~IRendererImpl() = default;
    };


}
