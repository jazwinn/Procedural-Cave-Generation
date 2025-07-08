#include "App.h"
#include <cmath>
#include <iostream>
#include <chrono>

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
		std::vector<std::pair<int, CellularAutomata>> activeCAs;
		std::vector<std::pair<int, PerlinWorm>> activePerlins;
		bool simulate = false;

		//initialize bsp
		float voxelSize = 1.f;
		glm::vec3 worldSize = glm::vec3{ 100,100,100 };
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


			if (ImGui::Button("Generate Cave")) {

				bsp.clear();
				activeCAs.clear();
				activePerlins.clear();
				voxel.clearVoxel();

				auto start = std::chrono::high_resolution_clock::now(); // Start timer

				if (binarySpacePartition) {
					bsp.Update();
					auto rooms = bsp.GetRooms();
					std::vector<bool> randomRooms = RandomClass::GenerateMaxTrue(seed, bsp.params.maxRoomCount, rooms.size());

					int count{};
					for (auto& room : rooms) {
						if (bsp.params.randomRooms && !randomRooms[count]) {
							count++;
							continue;
						}

						constexpr int subDim = 16;
						glm::vec3 extent = room.extent - glm::vec3(bsp.params.buffer);
						//do this to avoid negative extent
						extent = glm::max(extent, glm::vec3{ 1.f,1.f,1.f });

						int caKey = voxel.AddChunk(room.center.x, room.center.y, room.center.z, extent.x, extent.y, extent.z, voxelSize);
						auto chunks = voxel.GetChunk(caKey);

						glm::vec3 roomCenter = room.center + 0.5f * room.extent;
						glm::vec3 subWorldMin = roomCenter - glm::vec3{ subDim } *voxelSize * 0.5f;

						if (cellularAutomata) {
							CellularAutomata ca(seed);
							ca.SetChunk(chunks, subWorldMin, glm::vec3(subDim) * voxelSize);
							ca.SetSeeds({ room });

							activeCAs.emplace_back(caKey, std::move(ca));
						}

						if (perlinWorm) {

							if (!cellularAutomata) {
								chunks->FillChunk(SOLID);
							}

							PerlinWorm perlinWorm(seed);
							perlinWorm.SetChunk(chunks, subWorldMin, glm::vec3(subDim) * voxelSize);
							perlinWorm.SetRooms({ room });
							perlinWorm.ApplyPerlin();

							activePerlins.emplace_back(caKey, std::move(perlinWorm));
						}

						count++;
					}

					
				}
				else{
					int subDim = 100;
					int key = voxel.AddChunk(0, 0, 0, subDim, subDim, subDim, 1.f, SOLID);
					auto chunk = voxel.GetChunk(key);
					
					glm::vec3 roomCenter = glm::vec3{ 0.f,0.f,0.f } + 0.5f * glm::vec3(100);
					glm::vec3 subWorldMin = roomCenter - glm::vec3(16.f) * voxelSize * 0.5f;

					if (cellularAutomata) {
						CellularAutomata ca(seed);
						ca.SetChunk(chunk, subWorldMin, glm::vec3(subDim) * voxelSize);
						ca.SetSeeds({ BinarySpacePartition::Room{ roomCenter , glm::vec3(100) } });

						activeCAs.emplace_back(key, std::move(ca));
					}

					if (perlinWorm) {
						PerlinWorm perlinWorm(seed);
						perlinWorm.SetChunk(chunk, subWorldMin, glm::vec3(16) * voxelSize);
						perlinWorm.SetRooms({ BinarySpacePartition::Room{ roomCenter , glm::vec3(100) } });
						perlinWorm.Update();

						activePerlins.emplace_back(key, std::move(perlinWorm));
					}

				}

				auto end = std::chrono::high_resolution_clock::now(); // End timer
				auto duration = duration_cast<std::chrono::milliseconds>(end - start);
				lastAlgorithmRunTime = static_cast<float>(duration.count()) / 1000.0f; // Convert to seconds

				voxel.UpdateAllChunk();
			}

				

			bsp.DrawImgui();
			PerlinWorm::StaticDrawImgui();


			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Algorithms")) {
				ImGui::Checkbox("Binary Space Partioning", &binarySpacePartition);
				ImGui::Checkbox("Cellular Automata", &cellularAutomata);
				ImGui::Checkbox("Perlin Worm", &perlinWorm);
				ImGui::TreePop();
			}

			m_graphicPipeline.DrawImgui();


			if (ImGui::CollapsingHeader("Controls")) {
				ImGui::Checkbox("Run", &simulate);
				if (ImGui::Button("Step CA")) {
					for (auto& [key, ca] : activeCAs) {
						if (!ca.IsComplete()) {
							ca.Update();
							voxel.UpdateChunk(key);
						}
					}
				}
				ImGui::Separator();
			}


			if (simulate) {
				for (auto& [key, ca] : activeCAs) {
					if (!ca.IsComplete()) {
						std::cout << "CA updating" << std::endl;
						ca.Update();
						voxel.UpdateChunk(key);
					}
				}
			}



			








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