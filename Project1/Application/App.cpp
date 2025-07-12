#include "App.h"
#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>

#include "VoxelWorld.h"
#include "BinarySpacePartition.h"
#include "CellularAutomata.h"
#include "Perlin.hpp"




namespace app {
	App::App() : m_graphicPipeline(m_window), m_imgui(m_window.window, "#version 130") {}

	App::~App() {}

	int App::Run() {
		VoxelManager& voxel = m_graphicPipeline.Get_Voxel();

		unsigned int seed = 123456789; // Fixed seed for reproducibility

		//initialize Cellular Automata
		//std::vector<std::pair<int, PerlinWorm>> activePerlins;

		//initialize bsp
		float voxelSize = 1.f;
		glm::vec3 worldSize = glm::vec3{ 128,128,128 };
		glm::vec3 worldOrigin = glm::vec3{ 0,0,0 };	

		BinarySpacePartition bsp(seed);
		bsp.SetBounds(worldOrigin, worldSize);

		// Note to draw inside while loop
		Shapes& shape = m_graphicPipeline.Get_Shapes();
		Camera& camera = m_graphicPipeline.Get_Camera();

		//Algorithms to run
		bool binarySpacePartition = true;
		bool cellularAutomata = true;
		bool perlinWorm = true;
		
		bool simulateCA = false;
		//float caActionInterval = 0;
		//float caUpdateInterval = 3.0f;

		std::unique_ptr<CellularAutomata<VoxelWorld>> caPtr;
		std::unique_ptr<VoxelWorld> voxelWorldPtr;

		std::vector<BinarySpacePartition::Room> rooms;
		auto start = std::chrono::high_resolution_clock::now(); // Start timer

		float lastTime = static_cast<float>(glfwGetTime());
		float deltaTime = 0.0f;
		float lastAlgorithmRunTime = 0.0f;

		while (!glfwWindowShouldClose(m_window.window)) {
			float currentTime = static_cast<float>(glfwGetTime());
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			glfwPollEvents();
			glClearDepth(1.0);
			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		


			// UPDATE
			m_graphicPipeline.Update(deltaTime);
			m_imgui.Update();

			ImGui::Begin("Window");

			{
				ImGui::Text("Seed:");
				ImGui::SameLine();
				ImGui::PushItemWidth(80); // Set input width in pixels
				ImGui::InputScalar("##SeedInput", ImGuiDataType_U32, &seed);
				ImGui::PopItemWidth();
				ImGui::SameLine();

				if (ImGui::Button("Random")) {
					seed = rand();
				}
			}
				
			{
				static bool initTypeSolid = true;
				static bool runCaveGeneration = false;
				if (ImGui::Button("Generate Cave") && !runCaveGeneration) {
					bsp.clear();
					voxel.clearVoxel();
					voxelWorldPtr.reset();
					rooms.clear();
					caPtr.reset();

					start = std::chrono::high_resolution_clock::now(); // Start timer

					runCaveGeneration = true;

					BlockType initType = initTypeSolid ? SOLID : EMPTY;
					voxelWorldPtr = std::make_unique<VoxelWorld>(worldOrigin, worldSize.x, worldSize.y, worldSize.z, voxel, voxelSize, initType);

					if (cellularAutomata) {

						BlockType type = m_caParams.inverted ? SOLID : EMPTY;

						if (simulateCA) {
							caPtr = std::make_unique<CellularAutomata<VoxelWorld>>(*voxelWorldPtr, worldOrigin, worldSize, seed, type);
							//caActionInterval = 0.f;
						}
						else {
							CellularAutomata<VoxelWorld> inst(*voxelWorldPtr, worldOrigin, worldSize, seed, type);
							while (inst.GenerateCellularAutomata()) {
								static int stepCount = 0;
								stepCount++;
								if (stepCount > m_caParams.iterationCount) { break; }
							}
							caPtr.reset();
						}

					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Initialize Solid", &initTypeSolid);

				if (runCaveGeneration) {


					//update 
					bool done = false;
					if (simulateCA && caPtr) {

						auto startCA = std::chrono::high_resolution_clock::now(); // Start timer

						static bool firstRun = true;
						static bool runDone = false;
						if (firstRun && !runDone) {
							firstRun = false;
							std::thread([&]() {
								if (!caPtr->GenerateCellularAutomata()) {
									caPtr.reset();
									done = true;

								}

								runDone = true;
								//std::cout << "Cellular Automata finished." << std::endl;
								}).detach(); // Run CA update in a separate thread
						}


						//std::cout << "iterate" << std::endl;


						auto endCA = std::chrono::high_resolution_clock::now(); // End timer
						auto durationCA = duration_cast<std::chrono::milliseconds>(endCA - startCA);
						auto CAframeTime = static_cast<float>(durationCA.count()) / 1000.0f; // Convert to seconds


						//caActionInterval += deltaTime;

						// Step CA every 3 seconds (adjust timing as needed)
						if (runDone || done) {
							voxel.UpdateAllChunk();
							//caActionInterval = 0.f;  // Reset timer
							firstRun = true;
							runDone = false;
						}

					}
					else {
						done = true;
					}


					if (done) {



						if (binarySpacePartition) {
							bsp.Update();
							rooms = bsp.GetRooms();
						}
						else {
							//single room 
							rooms.push_back(BinarySpacePartition::Room{ glm::vec3{0.f,0.f,0.f}, bsp.GetSize() });
						}


						// Perlin Worm (run instantly)
						if (perlinWorm) {

							BlockType type = perlinParams.generateSolid ? SOLID : EMPTY;
							for (auto& room : rooms) {
								GeneratePerlin(*voxelWorldPtr, room.StartPoint, room.extent, seed, type);
							}
						}



						auto end = std::chrono::high_resolution_clock::now(); // End timer
						auto duration = duration_cast<std::chrono::milliseconds>(end - start);
						lastAlgorithmRunTime = static_cast<float>(duration.count()) / 1000.0f; // Convert to seconds

						if (voxel.config.marchingCube) {
							voxel.InsertMarchingCubeMesh(voxelWorldPtr->GenerateVertices());
						}

						runCaveGeneration = false;

						voxel.UpdateAllChunk();
					}




				}
			}
			

			bsp.DrawImgui();
			PerlinDrawImgui();

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Cellular Automata")) {
				ImGui::Checkbox("Simulate Steps", &simulateCA);
				CellularAutomataDrawImgui();
				ImGui::TreePop();
			}

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Algorithms")) {
				ImGui::Checkbox("Binary Space Partioning", &binarySpacePartition);
				ImGui::Checkbox("Cellular Automata", &cellularAutomata);
				ImGui::Checkbox("Perlin Worm", &perlinWorm);
				ImGui::TreePop();
			}

			m_graphicPipeline.DrawImgui();

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("Performance")) {
				ImGui::NewLine();
				ImGui::Text("Last Algorithm Run Time: %.3f seconds", lastAlgorithmRunTime);
				ImGui::NewLine();
				ImGui::Separator();

				float fps = 1.0f / deltaTime;
				static float FpsValues[90] = {};
				static int FpsValues_offset = 0;
				static double refresh_time = 0.0;

				if (refresh_time == 0.0) {
					refresh_time = ImGui::GetTime();
				}

				// Create data at fixed 60 Hz rate
				while (refresh_time < ImGui::GetTime())
				{
					static float phase = 0.0f;
					FpsValues[FpsValues_offset] = fps;
					FpsValues_offset = (FpsValues_offset + 1) % IM_ARRAYSIZE(FpsValues);
					phase += 0.10f * static_cast<float>(FpsValues_offset);
					refresh_time += 1.0f / 60.0f;
				}


				float average = 0.0f;
				for (int n = 0; n < IM_ARRAYSIZE(FpsValues); n++) {
					average += FpsValues[n];
				}

				average /= (float)IM_ARRAYSIZE(FpsValues);
				char overlay[32];
				snprintf(overlay, sizeof(overlay), "FPS %f", average);
				float minFps = FLT_MAX;
				float maxFps = -FLT_MAX;

				for (int n = 0; n < IM_ARRAYSIZE(FpsValues); n++) {
					if (FpsValues[n] < minFps) {
						minFps = FpsValues[n];
					}
					if (FpsValues[n] > maxFps) {
						maxFps = FpsValues[n];
					}
				}

				float yMin = minFps - 1.0f;
				float yMax = maxFps + 1.0f;

				ImGui::PlotLines("##", FpsValues, IM_ARRAYSIZE(FpsValues), FpsValues_offset, overlay, yMin, yMax, ImVec2(260.f, 120.0f));


			}

			ImGui::End();
			// DRAW
			bsp.Draw(shape, camera.GetViewProjectionMatrix(), glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			m_graphicPipeline.Draw();
			m_imgui.Render();

			glfwSwapBuffers(m_window.window);
		}

		return 0;
	}
}