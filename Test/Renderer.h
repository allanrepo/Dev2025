#pragma once
#include "IRenderer.h"
#include "IRendererImpl.h"

namespace graphics::renderer
{
    class Renderer: public graphics::renderer::IRenderer
    {
    private:
        std::unique_ptr<graphics::renderer::IRendererImpl> impl;

    public:
        Renderer(std::unique_ptr<graphics::renderer::IRendererImpl> pImpl);
        virtual ~Renderer() = default;

        virtual std::string GetTypeName() const override final;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        // Releases all sprite rendering resources
        virtual void ShutDown() override final;

        // Initializes the sprite renderer
        virtual bool Initialize() override final;

        // Begins a new sprite rendering batch
        virtual void Begin() override final;

        // Ends the current sprite rendering batch
        virtual void End() override final;

        // clipping region for rendering
		virtual void SetClipRegion(const math::geometry::RectF& region) override final;
		virtual void EnableClipping(const bool enable) override final;

        // Draws a colored quad at the specified position, size, and rotation
        virtual void Draw(
            const float x, const float y,               // Top-left screen position
            const float width, const float height,      // Sprite dimensions
            const float red, const float green,         // RGBA color components
            const float blue, const float alpha,
            const float rotation                        // Rotation in radians
        ) override final;

        // Draws a colored quad at the specified position, size, and rotation
        virtual void Draw(
            const spatial::PosF pos,                                 // Top-left screen position
            const spatial::SizeF size,                               // Sprite dimensions
            const graphics::ColorF color,                                   // RGBA color tint
            const float rotation                                                    // Rotation in radians
        ) override final;

        // Draws a string using a font atlas at the specified position and color
        virtual void DrawText(
            const std::shared_ptr<graphics::resource::IFontAtlas>& font, // Font atlas
            const std::string& text,                    // Text to render
            const float x, const float y,               // Top-left screen position
            const float red, const float green,         // RGBA color
            const float blue, const float alpha
        ) override final;

        // Draws a single character using a font atlas with color and rotation
        virtual void DrawChar(
            const std::shared_ptr<graphics::resource::IFontAtlas>& font, // Font atlas
            const unsigned char character,            // Character to render
            const float x, const float y,             // Top-left screen position
            const float red, const float green,       // RGBA color
            const float blue, const float alpha,
            const float rotation                      // Rotation in radians
        ) override final;

        // Draws a renderable quad with color tint and rotation
        virtual void DrawRenderable(
            const std::shared_ptr<graphics::renderable::IRenderable>& renderable,   // renderable object
            const spatial::PosF pos,                                 // Top-left screen position
            const spatial::SizeF size,                               // Sprite dimensions
            const graphics::ColorF color,                                   // RGBA color tint
            const float rotation                                                    // Rotation in radians
        ) override final;

        // draw an actor 
        virtual void Draw(
            const component::IActor& actor,                                             // actor object
            const graphics::ColorF tint = graphics::ColorF{ 1,1,1,1 },  // RGBA color tint
            const float rotation = 0                                                    // Rotation in radians
        ) override final;

        // Draws a renderable quad with color tint and rotation
        virtual void DrawRenderable(
            const graphics::renderable::IRenderable& renderable,                    // renderable object
            const spatial::PosF pos,                                 // Top-left screen position
            const spatial::SizeF size,                               // Sprite dimensions
            const graphics::ColorF color,                                   // RGBA color tint
            const float rotation                                                    // Rotation in radians
		) override final;
    };


}


