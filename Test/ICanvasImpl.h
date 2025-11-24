#pragma once

#include <cstdint> // For fixed-width integer types like uint32_t
#include <string>
#include "ICanvas.h"

namespace graphics::renderer
{
    // Interface for platform-specific renderer implementations
    class ICanvasImpl: public ICanvas
    {
    public:
        // Virtual destructor for safe polymorphic cleanup
        virtual ~ICanvasImpl() = default;

    };
}
