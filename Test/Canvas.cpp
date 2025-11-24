#include "Canvas.h"
#include <memory>
#include <string>
#include "ContextCache.h"

graphics::renderer::Canvas::Canvas(std::unique_ptr<graphics::renderer::ICanvasImpl> pImpl)
    : impl(std::move(pImpl))
{
}

bool graphics::renderer::Canvas::Initialize(void* pWindowHandle)
{
    return impl->Initialize(pWindowHandle);
}

void graphics::renderer::Canvas::Resize(uint32_t uiWidth, uint32_t uiHeight)
{
    impl->Resize(uiWidth, uiHeight);
}

void graphics::renderer::Canvas::ShutDown()
{
    impl->ShutDown();
}

void graphics::renderer::Canvas::Begin()
{
    impl->Begin();
}

void graphics::renderer::Canvas::End()
{
    impl->End();
}

void graphics::renderer::Canvas::Clear(float fRed, float fGreen, float fBlue, float fAlpha)
{
    impl->Clear(fRed, fGreen, fBlue, fAlpha);
}

void graphics::renderer::Canvas::SetViewPort(float uiX, float uiY, float uiWidth, float uiHeight)
{
    impl->SetViewPort(uiX, uiY, uiWidth, uiHeight);
}

void graphics::renderer::Canvas::SetViewPort()
{
    impl->SetViewPort();
}

std::string graphics::renderer::Canvas::GetTypeName() const
{
    return impl->GetTypeName();
}
