#pragma once
#include "Engine.h"
#include "Sprite.h"
#include "SpriteFactory.h"
#include "IDrawableSurface.h"
#include "DrawableSurface.h"
#include "TextureFactory.h"

class StressTestRendererWithEngine
{
private:
	struct Quad
	{
		spatial::PosF pos;
		spatial::SizeF size;
		graphics::ColorF color;
	};
	std::vector<Quad> quads;

	std::unique_ptr<graphics::resource::ISprite> m_sprite;
	engine::Engine m_engine;
	std::shared_ptr<graphics::resource::IDrawableSurface> surface;

public:
	StressTestRendererWithEngine() :
		m_engine("DirectX11", "Batch")
	{
		m_engine.OnStart += event::Handler(this, &StressTestRendererWithEngine::OnStart);
		m_engine.OnUpdate += event::Handler(this, &StressTestRendererWithEngine::OnUpdate);
		m_engine.OnRender += event::Handler(this, &StressTestRendererWithEngine::OnRender);
		m_engine.OnResize += event::Handler(this, &StressTestRendererWithEngine::OnResize);

		m_engine.Run();
	}

	void OnResize(size_t width, size_t height)
	{
		CalculateQuads(spatial::SizeF{ static_cast<float>(width), static_cast<float>(height) });
	}

	void OnUpdate(float delta)
	{
	}

	void OnStart()
	{
		// we can trust these lookup tables are already registered in cache by the engine

		// register our sprite name to corresponding sprite atlas key
		cache::Repository<cache::Dictionary<>>::Instance().Get("SpriteToAtlasMap").Register("debug_character_spriteatlas_6x8", "debug_character_spriteatlas_6x8.png");
		cache::Repository<cache::Dictionary<>>::Instance().Get("SpriteToAtlasMap").Register("StressTestImageLarge", "StressTestImage_64x64.png");
		cache::Repository<cache::Dictionary<>>::Instance().Get("SpriteToAtlasMap").Register("StressTestImageSmall", "StressTestImage_1x1.png");

		// register our image file to atlas UVs lookup table into cache. our atlas UV's are stored in csv file
		cache::Repository<cache::Dictionary<>>::Instance().Get("AtlasToUVRectsMap").Register("debug_character_spriteatlas_6x8.png", "debug_character_spriteatlas_6x8.csv");
		cache::Repository<cache::Dictionary<>>::Instance().Get("AtlasToUVRectsMap").Register("StressTestImage_64x64.png", "StressTestImage.csv");
		cache::Repository<cache::Dictionary<>>::Instance().Get("AtlasToUVRectsMap").Register("StressTestImage_1x1.png", "StressTestImage.csv");

		// create a sprite using the factory
		m_sprite = std::make_unique<graphics::resource::Sprite>(graphics::factory::SpriteFactory::Get("debug_character_spriteatlas_6x8", 0, math::VecF{}));

		// create an image surface for alternate rendering
		surface = std::make_shared<graphics::resource::DrawableSurface>(graphics::factory::TextureFactory::Create());
		surface->Initialize(64, 64);
		surface->Begin();
		surface->Clear(1, 1, 1, 1);
		surface->End();
	}

	void OnRender()
	{
		//m_engine.GetRenderer().DrawRenderable(
		//	*m_sprite, 
		//	spatial::PosF{ 100, 100 },
		//	spatial::SizeF{ m_sprite->GetWidth(), m_sprite->GetHeight() },
		//	graphics::ColorF{ 1, 1, 1, 1 },
		//	0);

		// 20250906: @50k  quads of 'debug_character_spriteatlas_6x8', @1400x900, it renders at least  225 FPS  
		// 20250906: @20k  quads of 'debug_character_spriteatlas_6x8', @1400x900, it renders at least  550 FPS  
		// 20250906: @100k quads of 'debug_character_spriteatlas_6x8', @1400x900, it renders at least  105 FPS  
		// 20250906: @10k  quads of 'debug_character_spriteatlas_6x8', @1400x900, it renders at least 1100 FPS  
		float j = 0;
		float peek = m_engine.GetTimer().Peek();
		for (Quad i : quads)
		{
			m_engine.GetRenderer().DrawRenderable(
				*m_sprite,
				i.pos,
				i.size,
				i.color,
				peek / j
			);
			j += 0.1f;
			if (j > 5) j = 0;
		}
	}

	void CalculateQuads(spatial::SizeF size)
	{
#pragma region // calculate quads
		{
			unsigned int row = 10;
			unsigned int col = 10;
			float max = 10000;

			quads.clear();
			float width = (float)size.width / col;
			float height = (float)size.height / row;
			float r = 0;
			float g = 0;
			float b = 0;
			float frames = max / (row * col);
			for (int k = 0; k < frames; k++)
			{
				for (unsigned int i = 0; i < row; i++)
				{
					for (unsigned int j = 0; j < col; j++)
					{
						spatial::PosF pos{ width * j, height * i };
						Quad qi;
						qi.pos = spatial::PosF{ width * j, height * i };
						qi.size = spatial::SizeF{ width, height };
						qi.color = graphics::ColorF{ r, g, b, 0.5f };
						quads.push_back(qi);
						r += 0.01f;
						g += 0.05f;
						b += 0.09f;
						r = r > 1 ? r -= 1 : r;
						g = g > 1 ? g -= 1 : g;
						b = b > 1 ? b -= 1 : b;
					}
				}
			}
		}
#pragma endregion
	}
};

