#pragma once
#pragma once
#include "Engine.h"
#include "Input.h"
#include "Logger.h"
#include "Tile.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include "ActorState.h"
#include "IActor.h"
#include "ActorFactory.h"
#include "DrawableSurface.h"
#include "ImageSurface.h"
#include "TextureFactory.h"

class TestSaveTextureToFile
{
private:
	engine::Engine m_engine;
	std::unique_ptr<graphics::resource::DrawableSurface> m_surface;

public:
	TestSaveTextureToFile() :
		m_engine("DirectX11", "Batch")
	{
		m_engine.OnStart += event::Handler(this, &TestSaveTextureToFile::OnStart);
		m_engine.OnUpdate += event::Handler(this, &TestSaveTextureToFile::OnUpdate);
		m_engine.OnRender += event::Handler(this, &TestSaveTextureToFile::OnRender);
		m_engine.OnResize += event::Handler(this, &TestSaveTextureToFile::OnResize);

		input::Input::Instance().OnKeyDown += event::Handler(this, &TestSaveTextureToFile::OnKeyDown);
		input::Input::Instance().OnMouseDown += event::Handler(this, &TestSaveTextureToFile::OnMouseDown);
		input::Input::Instance().OnMouseMove += event::Handler(this, &TestSaveTextureToFile::OnMouseMove);
		input::Input::Instance().OnMouseUp += event::Handler(this, &TestSaveTextureToFile::OnMouseUp);

		m_engine.Run();
	}

	void OnResize(size_t width, size_t height)
	{
	}

	void OnMouseMove(int x, int y)
	{
	}

	void OnMouseDown(int btn, int x, int y)
	{
	}

	void OnMouseUp(int btn, int x, int y)
	{
	}

	void OnKeyDown(int key)
	{
		LOG("OnKeyDown Key: " << key);
	}


	void OnStart()
	{
		std::vector<std::wstring> filenames =
		{
			L"isometric_Mini-Crusader/attack/crusader_attack_00000.png",
			L"isometric_Mini-Crusader/attack/crusader_attack_00001.png",
			L"isometric_Mini-Crusader/attack/crusader_attack_00002.png",
			L"isometric_Mini-Crusader/attack/crusader_attack_00003.png",
		};

		// create an image surface
		std::unique_ptr<graphics::resource::ITexture> tex = graphics::factory::TextureFactory::Create();
		graphics::resource::ITexture* pTex = tex.get();
		m_surface = std::make_unique<graphics::resource::DrawableSurface>(std::move(tex));
		m_surface->Initialize(1024, 1024);
		m_surface->Begin();
		m_surface->Clear(1, 1, 1, 1);

		m_engine.GetRenderer().Begin();
		float width = 0;
		float height = 0;
		for(const auto& fn : filenames)
		{
			graphics::resource::ImageSurface imgSurface(graphics::factory::TextureFactory::Create());
			imgSurface.Initialize(fn.c_str());
			m_engine.GetRenderer().DrawRenderable(imgSurface, { width, height }, imgSurface.GetSize(), { 1, 1, 1, 1 }, 0);

			width += imgSurface.GetWidth();
		}
		m_engine.GetRenderer().End();

		//graphics::resource::ImageSurface imgSurface(graphics::factory::TextureFactory::Create());
		//imgSurface.Initialize(L"isometric_Mini-Crusader/attack/crusader_attack_00000.png");
		//m_engine.GetRenderer().Begin();
		//m_engine.GetRenderer().DrawRenderable(imgSurface, { 0, 0 }, imgSurface.GetSize(), { 1, 1, 1, 1 }, 0);
		//m_engine.GetRenderer().Draw({ 50, 50 }, { 200, 200 }, { 1, 0, 0, 0.5f }, 0);
		m_engine.GetRenderer().End();
		m_surface->End();

		pTex->SaveToFile(L"output.png");
	}

	void OnUpdate(float delta)
	{
	}

	void OnRender()
	{
		m_engine.GetRenderer().DrawRenderable(*m_surface.get(), { 20, 20 }, m_surface->GetSize(), { 1, 1, 1, 1 }, 0);
	}
};

