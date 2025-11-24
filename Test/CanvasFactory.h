#pragma once
#include "Canvas.h"
#include "DX11CanvasImpl.h"
#include "Singleton.h"
#include "Factory.h"
#include "ContextCache.h"
#include <memory>
#include "Repository.h"
#include "Dictionary.h"

using namespace graphics::dx11;

namespace graphics
{
	class CanvasFactory
    {
    public:
        static std::unique_ptr<graphics::renderer::ICanvas> Create()
        {
			// get environment config from cache
            std::string typeName = 
                cache::Repository<cache::Dictionary<>>::Instance().Has("EnvironmentConfig") ?                       // do we have environment config?
                cache::Repository<cache::Dictionary<>>::Instance().Get("EnvironmentConfig").Has("API")?             // do we have API field in environment config?
                cache::Repository<cache::Dictionary<>>::Instance().Get("EnvironmentConfig").Get("API") :            // yes we have API field. let's get it
                graphics::dx11::renderer::DX11CanvasImpl::TypeName:                                                 // no API field in environment config, fallback to DX11
                graphics::dx11::renderer::DX11CanvasImpl::TypeName;                                                 // no config, fallback to DX11

            static bool loaded = false;
            if (!loaded)
            {
                core::Factory<std::string, graphics::renderer::Canvas>::Instance().Register(
                    graphics::dx11::renderer::DX11CanvasImpl::TypeName, []() 
                    { 
                        return std::make_unique<graphics::renderer::Canvas>(std::make_unique<graphics::dx11::renderer::DX11CanvasImpl>());
                    });

                loaded = true;
            }
            return core::Factory <std::string, graphics::renderer::Canvas> ::Instance().Create(typeName);
        }
    };
}


