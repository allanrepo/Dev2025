#pragma once
#include "Factory.h"
#include <memory>
#include "Sprite.h"
#include <string>
#include "Dictionary.h"
#include "Repository.h"
#include "Logger.h"
#include "CSVFile.h"
#include "ISpriteAtlas.h"
#include "SpriteAtlasFactory.h"

namespace graphics::factory
{
    class SpriteFactory
    {
    public:
        static graphics::resource::Sprite Get(const std::string& key, int index, math::VecF normRenderOffset)
        {
			// check lookup table for sprite key to atlas file pair. can't proceed if not found
            // TODO: consider returning a fallback sprite instead of throwing exception. this will throw exception if the dictionary does not exist
            cache::Dictionary<>& spriteToAtlasLookup = cache::Repository<cache::Dictionary<>>::Instance().Get("SpriteToAtlasMap");

			// get atlas file name using sprite key. can't proceed if not found. this will throw exception if not found.
            // TODO: consider returning a fallback atlas instead of throwing exception. this will throw exception if the dictionary does not exist
            std::string& atlasName = spriteToAtlasLookup.Get(key);

            // now we know the atlas file name. check if it is already registered in the sprite atlas Repository
            if (!cache::Repository<graphics::resource::ISpriteAtlas>::Instance().Has(atlasName))
            {
				// TODO: we should wrap this into a method in sprite atlas Repository. this is too long
                
                // we don't have the atlas registered. let's create it now and register to our cache 
                cache::Repository<graphics::resource::ISpriteAtlas>::Instance().Register(atlasName, graphics::factory::SpriteAtlasFactory::Create());

				// get it from Repository and initialize it with the atlas file name
                // TODO: consider having a fallback atlas rather than let this throw exception
                graphics::resource::ISpriteAtlas& spriteAtlas = cache::Repository<graphics::resource::ISpriteAtlas>::Instance().Get(atlasName);

                if (!spriteAtlas.Initialize(std::wstring(atlasName.begin(), atlasName.end()).c_str()))
                {
                    // TODO: consider returning a fallback sprite instead of throwing exception
                    LOG("Failed to initialize sprite atlas with image file: " << atlasName);
					throw std::exception("Failed to initialize sprite atlas with image file.");
                }

                // check first if we have the AtlasToUVRectsMap lookup table. we need this to find the csv file that contains the UVs for the atlas
                // TODO: this is gonna be tricky, but if AtlasToUVRectsMap, we're kinda fucked. figure out how to have fallback here, or this will throw exception
                cache::Dictionary<>& atlasToUVsLookup = cache::Repository<cache::Dictionary<>>::Instance().Get("AtlasToUVRectsMap");

                // get UVs csv file name. if we don't have it, we cannot proceed. this will throw an exception
                // TODO: again, a tricky one, because we need UV information. if this fails, it throws exception. how can we have fallback instead? is it even possible?
                std::string& UVsName = atlasToUVsLookup.Get(atlasName);

                // read CSV file and add sprites to atlas
                utilities::fileio::CSVFile csvFile(UVsName, ',');
                if (!csvFile.read())
                {
                    throw std::runtime_error("Failed to read CSV file");
                }
                int rowCount = static_cast<int>(csvFile.GetRowCount());
                for (int row = 0; row < rowCount; row++)
                {
                    math::geometry::RectF rect
                    {
                        csvFile.GetValue<float>(row, 0), // left
                        csvFile.GetValue<float>(row, 1), // top
                        csvFile.GetValue<float>(row, 2), // right
                        csvFile.GetValue<float>(row, 3) // bottom
                    };
                    spriteAtlas.AddUVRect(rect);
                }

				// at this point, we should have the sprite atlas initialized and populated with sprites. we are ready to get the sprite using the index and return it
            }

			// if sprite atlas is already registered, get it. make sure we got it. can't proceed if not found
            // TODO: consider having a fallback atlas rather than let this throw exception
            graphics::resource::ISpriteAtlas& spriteAtlas = cache::Repository<graphics::resource::ISpriteAtlas>::Instance().Get(atlasName);

            //// the render offset we received is normalized, because the caller that is creating sprite does not know yet about the sprite atlas to be used
            //// so we conver to absolute value here
            //normRenderOffset.x *= spriteAtlas->GetWidth();
            //normRenderOffset.y *= spriteAtlas->GetHeight();

			// finally, get the sprite from the sprite atlas using the index and return it
			return graphics::resource::Sprite(&spriteAtlas, spriteAtlas.GetUVRect(index), normRenderOffset);
        }
    };
}

