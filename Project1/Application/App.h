#pragma once

#include "window.h"
#include "GraphicPipeline.h"
#include "ImguiHandler.h"

namespace app{
	class App
	{
	public:
		App();
		~App();

		int Run();

	private:

		window::Window m_window;
		graphic::GraphicPipeline m_graphicPipeline;
		imgui::ImguiHandler m_imgui;

	};

}

