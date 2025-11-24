#pragma once

#include "Window.h"
#include "CanvasFactory.h"
#include "RendererFactory.h"
#include "TextureFactory.h"
#include "Logger.h"
#include "FontAtlas.h"
#include "DrawableSurface.h"
#include "Input.h"
#include "StopWatch.h"

using namespace math::geometry;
using namespace graphics;

class FrameRate
{
private:
	timer::StopWatch stopWatch;
	bool first = true;
	float elapsedTime = 0;
	float duration = 0.5;
	unsigned int numFrames = 0;
	float fps = 0;
public:
	event::Event<float> OnFrameRate;

	const float GetFPS() const
	{
		return fps;
	}

	void Update()
	{
		if (first)
		{
			stopWatch.Start();
			elapsedTime = 0;
			numFrames = 0;
			first = false;
		}
		elapsedTime += stopWatch.Lap();
		numFrames++;

		if (elapsedTime > duration)
		{
			fps = numFrames / elapsedTime;
			OnFrameRate(fps);
			elapsedTime = 0;
			numFrames = 0;
		}
	}

};

class TestInput
{
private:
	std::unique_ptr<Win32::Window> window;
	std::unique_ptr<graphics::renderer::ICanvas> canvas;
	std::unique_ptr<graphics::renderer::IRenderer> RendererImmediate;
	std::unique_ptr<graphics::renderer::IRenderer> RendererBatch;
	std::shared_ptr<graphics::resource::DrawableSurface> surface;
	std::shared_ptr<graphics::resource::IFontAtlas> fontAtlasComicSansMS;
	input::Input input;
	timer::StopWatch timer;
	float elapsedTimeLastFrame;
	unsigned int numFrames;
	float elapsedTime;
	float frameRate;
	FrameRate fr;
	double fps;

public:
	TestInput()
	{
		Win32::Window::OnInitialize += event::Handler(this, &TestInput::OnInitialize);
		Win32::Window::OnExit += event::Handler(this, &TestInput::OnExit);
		Win32::Window::OnIdle += event::Handler(this, &TestInput::OnIdle);

		Win32::Window::Run();
	}

	// function that will be called just before we enter into message loop
	void OnInitialize()
	{
		// create our window here
		window = std::make_unique<Win32::Window>();
		window->OnClose += event::Handler(this, &TestInput::OnWindowClose);
		window->OnCreate += event::Handler(this, &TestInput::OnWindowCreate);
		window->OnSize += event::Handler(this, &TestInput::OnWindowSize);
		window->Create(L"Test Bedding", 1400, 900);
		window->OnWindowMessage += event::Handler(&input, &input::Input::ProcessWin32Message);

		input.OnKeyDown += event::Handler(this, &TestInput::OnKeyDown);
		input.OnKeyUp += event::Handler(this, &TestInput::OnKeyUp);
		input.OnMouseDown += event::Handler(this, &TestInput::OnMouseDown);
		input.OnMouseUp += event::Handler(this, &TestInput::OnMouseUp);
		input.OnMouseMove += event::Handler(this, &TestInput::OnMouseMove);
	}

	void OnKeyDown(int key)
	{
		LOG("OnKeyDown Key: " << key);
	}

	void OnKeyUp(int key)
	{
		LOG("OnKeyUp Key: " << key);
	}

	void OnMouseDown(int btn, int x, int y)
	{
		LOG("OnMouseDown btn: " << btn << " x: " << x << " y:" << y);

	}

	void OnMouseUp(int btn, int x, int y)
	{
		LOG("OnMouseUp btn: " << btn << " x: " << x << " y:" << y);

	}

	void OnMouseMove(int x, int y)
	{
		LOG("OnMouseMove x: " << x << " y:" << y);
	}

	void OnFrameRate(float _fps)
	{
		fps = _fps;
	}

	// fun stuff. this is called on each loop of the message loop. this is where we draw!
	void OnIdle()
	{
		elapsedTime += timer.Lap();
		numFrames++;
		if (elapsedTime > 1.0)
		{
			frameRate = numFrames / elapsedTime;
			numFrames = 0;
			elapsedTime = 0;
		}

		fr.Update();

		input.Update();

		// start the canvas. we can draw from here
		canvas->Begin();
		{
			canvas->Clear(0.2f, 0.2f, 1.0f, 1.0f);
			canvas->SetViewPort();

			surface->Begin();
			surface->Clear(1, 1, 1, 1);
			{
				std::unique_ptr<graphics::renderer::IRenderer> localRenderer = graphics::factory::RendererFactory::Create("Immediate");
				localRenderer->Initialize();

				// let's begin drawing stuff in the texture
				localRenderer->Begin();
				{
					localRenderer->Draw(10.0f, 10.0f, 80.0f, 80.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0); // Draw a rectangle to the texture
					localRenderer->Draw(50.0f, 40.0f, 120.0f, 80.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0); // Draw a rectangle to the texture
					localRenderer->Draw(70.0f, 50.0f, 80.0f, 100.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0); // Draw a rectangle to the texture

					localRenderer->Draw(5.0f, 155.0f, 40.0f, 40.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0); // Draw a rectangle to the texture
					localRenderer->DrawText(fontAtlasComicSansMS, "Hello", 0.0f, 150.0f, 1, 0, 0, 1);
					localRenderer->Draw(20.0f, 160.0f, 80.0f, 30.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0); // Draw a rectangle to the texture
				}
				localRenderer->End();
			}
			surface->End();


			// render sprites using batch renderer. 
			RendererBatch->Begin();
			{
				RendererBatch->DrawRenderable(surface, spatial::PosF{ 5.0f, 5.0f }, spatial::SizeF{ surface->GetWidth(), surface->GetHeight() }, ColorF{ 1,1,1,1 }, 0.0f);
				RendererBatch->DrawText(fontAtlasComicSansMS, std::to_string(frameRate), 5, 400, 1, 1, 1, 1);
				RendererBatch->DrawText(fontAtlasComicSansMS, std::to_string(fps), 5, 420, 1, 1, 1, 1);
			}
			RendererBatch->End();

		}
		// end the canvas. we don't draw anything past this.
		canvas->End();
	}

	void OnExit()
	{

	}

	// when window is created
	void OnWindowCreate(void* hWnd)
	{
		// create dx11 canvas
		canvas = graphics::CanvasFactory::Create();
		canvas->Initialize(hWnd);

		// create sprite renderer immediate
		RendererImmediate = graphics::factory::RendererFactory::Create("Immediate");
		RendererImmediate->Initialize();

		// create sprite renderer batch
		RendererBatch = graphics::factory::RendererFactory::Create("Batch");
		RendererBatch->Initialize();

		fontAtlasComicSansMS = std::make_shared<graphics::resource::FontAtlas>(graphics::factory::TextureFactory::Create());
		fontAtlasComicSansMS->Initialize("Comic Sans MS", 16);

		// create a drawable texture
		surface = std::make_shared<graphics::resource::DrawableSurface>(graphics::factory::TextureFactory::Create());
		surface->Initialize(300, 200);

		timer.Start();
		elapsedTimeLastFrame = 0;
		numFrames = 0;
		elapsedTime = 0;
		frameRate = 0;

		fr.OnFrameRate += event::Handler(this, &TestInput::OnFrameRate);
	}

	void OnWindowClose()
	{

	}

	void OnWindowSize(size_t nWidth, size_t nHeight)
	{
		LOG("window resize width = " << nWidth << " height = " << nHeight);
		canvas->Resize(static_cast<uint32_t>(nWidth), static_cast<uint32_t>(nHeight));
	}

};

