#pragma once

#include "window.h"
#include "../Graphics/GraphicPipeline.h"

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

	};

}

