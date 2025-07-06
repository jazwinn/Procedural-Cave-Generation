#pragma once

#include "BinarySpacePartition.h"
#include "CellularAutomata.h"
#include "window.h"
#include "GraphicPipeline.h"
#include "ImguiHandler.h"
#include "RandomAlgo.h"

namespace app {
	class App {
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