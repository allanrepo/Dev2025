//#pragma once
//
//#include "Window.h"
//#include "CanvasFactory.h"
//#include "RendererFactory.h"
//#include "TextureFactory.h"
//#include "Logger.h"
//#include "FontAtlas.h"
//#include "DrawableSurface.h"
//#include "ImageSurface.h"
//#include "Timer.h"
//#include "Animation.h"
//#include "ISpriteAtlas.h"
//#include "IRenderable.h"
//#include "ContextCache.h"
//#include "ISprite.h"
//#include "Actor.h"
//#include "CSVFile.h"
//#include "SpriteFactory.h"
//#include "Dictionary.h"
//
//using namespace math::geometry;
//using namespace graphics;
//
//
//class TestSprite
//{
//private:
//	struct Quad
//	{
//		spatial::PosF pos;
//		spatial::SizeF size;
//		graphics::ColorF color;
//	};
//	std::vector<Quad> quads;
//
//	std::unique_ptr<Win32::Window> window;
//	std::unique_ptr<graphics::renderer::ICanvas> canvas;
//	std::unique_ptr<graphics::renderer::IRenderer> RendererImmediate;
//	std::unique_ptr<graphics::renderer::IRenderer> RendererBatch;
//	std::shared_ptr<graphics::resource::DrawableSurface> surface;
//	timer::StopWatch stopwatch;
//	float elapsedTime = 0;
//	std::shared_ptr<graphics::resource::ISprite> m_sprite;
//	std::unique_ptr<engine::IActor> m_actor;
//	std::shared_ptr<graphics::resource::ISprite> m_sprite1;
//
//public:
//	TestSprite()
//	{
//		Win32::Window::OnInitialize += event::Handler(&TestSprite::OnInitialize, this);
//		Win32::Window::OnExit += event::Handler(&TestSprite::OnExit, this);
//		Win32::Window::OnIdle += event::Handler(&TestSprite::OnIdle, this);
//
//		Win32::Window::Run();
//	}
//
//	// function that will be called just before we enter into message loop
//	void OnInitialize()
//	{
//		// create our window here
//		window = std::make_unique<Win32::Window>();
//		window->OnClose += event::Handler(&TestSprite::OnWindowClose, this);
//		window->OnCreate += event::Handler(&TestSprite::OnWindowCreate, this);
//		window->OnSize += event::Handler(&TestSprite::OnWindowSize, this);
//		window->Create(L"Test Bedding", 1400, 900);
//	}
//
//	// when window is created
//	void OnWindowCreate(void* hWnd)
//	{
//		cache::Repository<cache::Dictionary<>>::Instance().Register("EnvironmentConfig", std::make_unique<cache::Dictionary<>>());
//		cache::Dictionary<>* environmentConfig = cache::Repository<cache::Dictionary<>>::Instance().Get("EnvironmentConfig");
//		if (!environmentConfig)
//		{
//			throw std::exception("Failed to access environment config lookup table.");
//		}
//		environmentConfig->Register("API", "DirectX11");
//		environmentConfig->Register("Render Mode", "Batch");
//
//
//		// create dx11 canvas
//		canvas = graphics::CanvasFactory::Create();
//		canvas->Initialize(hWnd);
//
//		// create sprite renderer immediate
//		RendererImmediate = graphics::factory::RendererFactory::Create("Immediate");
//		RendererImmediate->Initialize();
//
//		// create sprite renderer batch
//		RendererBatch = graphics::factory::RendererFactory::Create("Batch");
//		RendererBatch->Initialize();
//
//		// create an image surface
//		surface = std::make_shared<graphics::resource::DrawableSurface>(graphics::factory::TextureFactory::Create());
//		surface->Initialize(1, 1);
//		surface->Begin();
//		surface->Clear(1, 1, 1, 1);
//		surface->End();
//
//		// setup stopwatch to manage timing
//		stopwatch.OnLap += event::Handler(&TestSprite::OnLap, this);
//		stopwatch.Start();
//
//		// register our lookup table into cache. this is lookup table for sprite names to corresponding sprite atlas
//		cache::Repository<cache::Dictionary<>>::Instance().Register("SpriteToAtlasMap", std::make_unique<cache::Dictionary<>>());
//
//		// now let's get the lookup table from cache
//		cache::Dictionary<>* spriteToAtlas = cache::Repository<cache::Dictionary<>>::Instance().Get("SpriteToAtlasMap");
//		if (!spriteToAtlas)
//		{
//			throw std::exception("Failed to access SpriteToAtlasMap lookup table.");
//		}
//
//		// now let's register our sprite name to corresponding sprite atlas key
//		spriteToAtlas->Register("debug_character_spriteatlas_6x8", "debug_character_spriteatlas_6x8.png");
//
//		// register our image file to atlas UVs lookup table into cache. our atlas UV's are stored in csv file
//		cache::Repository<cache::Dictionary<>>::Instance().Register("AtlasToUVRectsMap", std::make_unique<cache::Dictionary<>>());
//
//		// now let's get the lookup table from cache
//		cache::Dictionary<>* atlasToUVs = cache::Repository<cache::Dictionary<>>::Instance().Get("AtlasToUVRectsMap");
//		if (!atlasToUVs)
//		{
//			throw std::exception("Failed to access AtlasToUVRectsMap lookup table.");
//		}
//
//		// now let's register our sprite name to corresponding sprite atlas key
//		atlasToUVs->Register("debug_character_spriteatlas_6x8.png", "debug_character_spriteatlas_6x8.csv");
//
//
//		m_actor = std::make_unique<engine::Actor>();
//
//		// let's make a sprite
//		//m_sprite1 = graphics::factory::SpriteFactory::Get("debug_character_spriteatlas_6x8", 0);
//
//
//
//	}
//
//	void OnLap(float delta)
//	{
//		elapsedTime += delta;
//
//		m_actor->Update(delta);
//	}
//
//	// fun stuff. this is called on each loop of the message loop. this is where we draw!
//	void OnIdle()
//	{
//		stopwatch.Lap<timer::milliseconds>();
//
//		// start the canvas. we can draw from here
//		canvas->Begin();
//		{
//			canvas->Clear(0.2f, 0.2f, 1.0f, 1.0f);
//			canvas->SetViewPort();
//
//			// render sprites using immediate renderer. renders a bunch of quads and text at the right side of screen
//			RendererImmediate->Begin();
//			{
//			}
//			RendererImmediate->End();
//
//			// render sprites using batch renderer. 
//			RendererBatch->Begin();
//			{
//				// TODO: ok source rect being absolute value and not normalized is messy. fix this back
//				//RendererBatch->DrawRenderable(
//				//	m_sprite,
//				//	spatial::PosF{ 50, 50 },
//				//	spatial::SizeF{ m_sprite->GetWidth(), m_sprite->GetHeight() },
//				//	graphics::ColorF{ 1,1,1,1 },
//				//	0);
//
//				RendererBatch->DrawRenderable(
//					*m_actor,
//					spatial::PosF{ 150, 150 },
//					spatial::SizeF{ m_actor->GetWidth(), m_actor->GetHeight() },
//					graphics::ColorF{ 1,1,1,1 },
//					0);
//
//				//RendererBatch->DrawRenderable(
//				//	*m_sprite1.get(),
//				//	spatial::PosF{ 250, 250 },
//				//	spatial::SizeF{ m_sprite1->GetWidth(), m_sprite1->GetHeight() },
//				//	graphics::ColorF{ 1,1,1,1 },
//				//	0);
//
//				//float j = 0;
//				//for (Quad i : quads)
//				//{
//				//	//SpriteBatch::SetVisibleRegion(vp);
//				//	RendererBatch->Draw(i.pos.x, i.pos.y, i.size.width, i.size.height, i.color.red, i.color.green, i.color.blue, i.color.alpha, elapsedTime / j);
//				//	//RendererBatch->DrawRenderable(surface, i.pos, i.size, i.color, elapsedTime / j);
//				//	j += 0.1f;
//				//	if (j > 5) j = 0;
//				//}
//
//
//			}
//			RendererBatch->End();
//
//		}
//		// end the canvas. we don't draw anything past this.
//		canvas->End();
//	}
//
//	void OnExit()
//	{
//
//	}
//
//	void OnWindowClose()
//	{
//
//	}
//
//	void OnWindowSize(size_t nWidth, size_t nHeight)
//	{
//		LOG("window resize width = " << nWidth << " height = " << nHeight);
//		canvas->Resize(static_cast<uint32_t>(nWidth), static_cast<uint32_t>(nHeight));
//
//		CalculateQuads(spatial::SizeF{ static_cast<float>(nWidth), static_cast<float>(nHeight) });
//	}
//
//	void CalculateQuads(spatial::SizeF size)
//	{
//#pragma region // calculate quads
//		{
//			unsigned int row = 10;
//			unsigned int col = 10;
//			float max = 50000;
//
//			quads.clear();
//			float width = (float)size.width / col;
//			float height = (float)size.height / row;
//			float r = 0;
//			float g = 0;
//			float b = 0;
//			float frames = max / (row * col);
//			for (int k = 0; k < frames; k++)
//			{
//				for (unsigned int i = 0; i < row; i++)
//				{
//					for (unsigned int j = 0; j < col; j++)
//					{
//						spatial::PosF pos{ width * j, height * i };
//						Quad qi;
//						qi.pos = spatial::PosF{ width * j, height * i };
//						qi.size = spatial::SizeF{ width, height };
//						qi.color = graphics::ColorF{ r, g, b, 0.5f };
//						quads.push_back(qi);
//						r += 0.01f;
//						g += 0.05f;
//						b += 0.09f;
//						r = r > 1 ? r -= 1 : r;
//						g = g > 1 ? g -= 1 : g;
//						b = b > 1 ? b -= 1 : b;
//					}
//				}
//			}
//		}
//#pragma endregion
//	}
//
//
//
//};
//
