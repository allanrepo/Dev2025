#pragma once
#include "ICanvas.h"
#include "ICanvasImpl.h"
#include <memory>

namespace graphics::renderer
{
    class Canvas : public graphics::renderer::ICanvas
    {
    public:
        explicit Canvas(std::unique_ptr<graphics::renderer::ICanvasImpl> pImpl);

        virtual bool Initialize(void* pWindowHandle) override final;
        virtual void Resize(uint32_t uiWidth, uint32_t uiHeight) override final;
        virtual void ShutDown() override final;                  

        virtual void Begin() override final;
        virtual void End() override final;

        virtual void Clear(float fRed, float fGreen, float fBlue, float fAlpha) override final;
        virtual void SetViewPort(float uiX, float uiY, float uiWidth, float uiHeight) override final;
        virtual void SetViewPort() override final;

        virtual std::string GetTypeName() const override final;

    private:
        std::unique_ptr<graphics::renderer::ICanvasImpl> impl;
    };
}

