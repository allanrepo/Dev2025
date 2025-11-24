#pragma once
#include <memory>
#include "Factory.h"
#include "TextureImpl.h"
#include "DX11TextureImpl.h"
#include "IRenderable.h"
#include "Dictionary.h"
#include "Repository.h"

// this is the right way to forward declare a class like ISpriteAtlas because in SpriteAtlasFactory, we are using it on std::unique_ptr
// it wants it like this
namespace graphics::resource 
{ 
    class ISpriteAtlas; 
} 

namespace graphics::factory
{
    class TextureFactory
    {
    public:
        static std::unique_ptr<graphics::resource::ITexture> Create()
        {
            // get environment config from cache
            std::string typeName =
                cache::Repository<cache::Dictionary<>>::Instance().Has("EnvironmentConfig") ?                       // do we have environment config?
                cache::Repository<cache::Dictionary<>>::Instance().Get("EnvironmentConfig").Has("API") ?             // do we have API field in environment config?
                cache::Repository<cache::Dictionary<>>::Instance().Get("EnvironmentConfig").Get("API") :            // yes we have API field. let's get it
                graphics::dx11::resource::DX11TextureImpl::TypeName :                                                 // no API field in environment config, fallback to DX11
                graphics::dx11::resource::DX11TextureImpl::TypeName;                                                 // no config, fallback to DX11

            static bool loaded = false;
            if (!loaded)
            {
                core::Factory<std::string, graphics::resource::ITexture>::Instance().Register(
                    graphics::dx11::resource::DX11TextureImpl::TypeName, []()
                    {
                        return std::make_unique<graphics::dx11::resource::DX11TextureImpl>();
                    });

                loaded = true;
            }
            return core::Factory <std::string, graphics::resource::ITexture>::Instance().Create(typeName);
        }
    }; 
}
