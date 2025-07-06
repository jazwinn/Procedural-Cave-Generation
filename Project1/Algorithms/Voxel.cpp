#include "Voxel.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
#include <array>


std::vector<Quad> Chunks::GenerateQuads() {
    std::vector<Quad> quads;

    // Process each face direction
    for (int face = 0; face < 6; face++) {
       // printf("Processing face %d\n", face);

        // For each face, we need to iterate through all blocks and check if that face should be rendered
        for (int x = 0; x < m_Width; x++) {
            for (int y = 0; y < m_Height; y++) {
                for (int z = 0; z < m_Depth; z++) {
                    if (shouldRenderFace(x, y, z, face)) {
                        // Calculate the actual position of this face in world space
                        Quad quad;
                        // Base position is the block position
                        int face_x = x;
                        int face_y = y;
                        int face_z = z;


                        // Apply world offset
                        quad.x = face_x;
                        quad.y = face_y;
                        quad.z = face_z;

                        quad.width = 1;
                        quad.height = 1;
                        quad.face = (Face)face;
                        quad.type = at(x, y, z);

                        //printf("Block (%d,%d,%d) Face %d -> Position (%d,%d,%d)\n",
                       //     x, y, z, face, quad.x, quad.y, quad.z);

                        quads.push_back(quad);
                    }
                }
            }
        }
    }

    //printf("Total quads generated: %zu\n", quads.size());
    return quads;
}

Voxel::Voxel(Shader& shader):
	m_modified{ false },
	m_Shader{ shader }
{

    std::vector<GLfloat> verticesQuad = {
        // positions (x, y, z)
        -0.5f, -0.5f, 0.0f,  // bottom-left
         0.5f, -0.5f, 0.0f,  // bottom-right
         0.5f,  0.5f, 0.0f,  // top-right
        -0.5f,  0.5f, 0.0f   // top-left
    };

    // Indices for the two triangles that make up the quad
    std::vector<GLuint> indicesQuad = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
    };

	// Initialize with identity matrix, for mesh initialization
	m_transforms.push_back(glm::mat4(1.0f)); 


    m_InstancedQuad = std::make_unique<Mesh>(verticesQuad, indicesQuad, m_transforms.size(), m_transforms, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	m_transforms.clear();
	m_InstancedQuad->GetInstanceVBO().UpdateData(m_transforms.data(), 0, 0);
}

void Voxel::UpdateChunk(int key)
{
	auto it = m_Chunks.find(key);
	if (it != m_Chunks.end()) {
		// Chunk exists, update it
		std::vector<Quad> quads;
		if (config.greedy) {
			quads = it->second->GenerateQuadsGreedy();
		}
		else {
			quads = it->second->GenerateQuads();
		}
		
		std::vector<glm::mat4x4> transforms;

		float scaleFactor = it->second->GetScale(); // Scale factor for the cubes
		glm::vec3 chunkPosition = it->second->GetPosition();

		for (const auto& quad : quads) {

			//Chunk transformation
			glm::mat4 model = glm::mat4(1.f);
			model = glm::translate(model, chunkPosition);
			model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		

			glm::vec3 faceOffset;
			switch (quad.face) {
			case 0: faceOffset = glm::vec3(0.5f					  , 0.5f * (quad.height - 1), 0.5f * (quad.width - 1)); break; // +X
			case 1: faceOffset = glm::vec3(-0.5f			      , 0.5f * (quad.height - 1), 0.5f * (quad.width - 1)); break; // -X
			case 2: faceOffset = glm::vec3(0.5f * (quad.width - 1), 0.5f				    , 0.5f * (quad.height - 1)); break; // +Y
			case 3: faceOffset = glm::vec3(0.5f * (quad.width - 1), -0.5f					, 0.5f * (quad.height - 1)); break; // -Y
			case 4: faceOffset = glm::vec3(0.5f * (quad.width - 1), 0.5f * (quad.height - 1), 0.5f); break; // +Z
			case 5: faceOffset = glm::vec3(0.5f * (quad.width - 1), 0.5f * (quad.height - 1), -0.5f); break; // -Z
			}


			model = glm::translate(model, glm::vec3(quad.x, quad.y, quad.z) + faceOffset);
		
		
			switch (quad.face) {
			case 0: // +X face (right)
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
				break;
			case 1: // -X face (left)
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
				break;
			case 2: // +Y face (top)
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
				break;
			case 3: // -Y face (bottom)
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
				break;
			case 4: // +Z face (front)
			
				break;
			case 5: // -Z face (back)
				model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
				break;
			};

			model = glm::scale(model, glm::vec3(quad.width, quad.height, 1.f));
		
			transforms.emplace_back(model);
		}



		m_ChunkTransforms[key] = transforms;

	}
	else {
		std::cout << "Chunk with key " << key << " does not exist." << std::endl;
        return;
		
	}
	m_modified = true;

}

void Voxel::UpdateAllChunk()
{
	for (const auto& chunk : m_Chunks) {
		UpdateChunk(chunk.first);
	}
}

void Voxel::DrawVoxel(const glm::mat4& vp, const glm::vec4& color, GLenum mode)
{
	m_Shader.Activate();
	//if modifed is true, updaet vbo
	if (m_modified) {
		//add all chunks transformation together
		m_transforms.clear();

		for (const auto& [key, transforms] : m_ChunkTransforms) {
			m_transforms.insert(m_transforms.end(), transforms.begin(), transforms.end());
		}

		m_InstancedQuad->SetInstanceCount(static_cast<GLuint>(m_transforms.size()));

		VAO& vao = m_InstancedQuad->GetVAO();
		VBO& instanceVBO = m_InstancedQuad->GetInstanceVBO();
		vao.Bind();
		instanceVBO.UpdateData(m_transforms.data(), m_transforms.size() * sizeof(glm::mat4x4), 0);

		m_modified = false;
	}

	if (m_transforms.empty()) return;

	m_Shader.setUniform("uniform_m2w", vp);
	m_Shader.setUniform("uniform_color", color);

	if (mode == GL_LINES) {

		//draw sold
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_InstancedQuad->Draw(GL_TRIANGLES);

		//draw wireframe
		m_Shader.setUniform("uniform_color", glm::vec4{0.f,0.f,0.f,1.f});
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, -1.0f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1.0f);
		m_InstancedQuad->Draw(GL_TRIANGLES);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
	else {
		m_InstancedQuad->Draw(mode);
	}
	
}

void Voxel::deleteChunk(int key)
{
	if (m_Chunks.find(key) != m_Chunks.end()) {
		m_Chunks.erase(key);
		m_ChunkTransforms.erase(key);
		m_modified = true;
	}
	else {
		std::cout << "Chunk with key " << key << " does not exist." << std::endl;
	}
}

void Voxel::clearVoxel()
{
	std::vector<int> keysToDelete;
	for (const auto& chunk : m_Chunks) {
		keysToDelete.push_back(chunk.first);
	}

	for (int key : keysToDelete) {
		deleteChunk(key);
	}
}



std::vector<Quad> Chunks::GenerateQuadsGreedy()
{
	std::vector<Quad> quads;

	// Process each face direction
	for (int face = 0; face < 6; face++) {

		int depth_coord, plane_coord1, plane_coord2;
		int depth_size, plane_size1, plane_size2;

		switch (face) {
		case 0: case 1: // +X, -X faces (iterate through X, meshing in YZ plane)
			depth_coord = 0; plane_coord1 = 1; plane_coord2 = 2;
			depth_size = m_Width; plane_size1 = m_Height; plane_size2 = m_Depth;
			break;
		case 2: case 3: // +Y, -Y faces (iterate through Y, meshing in XZ plane)
			depth_coord = 1; plane_coord1 = 0; plane_coord2 = 2;
			depth_size = m_Height; plane_size1 = m_Width; plane_size2 = m_Depth;
			break;
		case 4: case 5: // +Z, -Z faces (iterate through Z, meshing in XY plane)
			depth_coord = 2; plane_coord1 = 0; plane_coord2 = 1;
			depth_size = m_Depth; plane_size1 = m_Width; plane_size2 = m_Height;
			break;
		}

		// Slice through each layer perpendicular to the face normal
		for (int depth = 0; depth < depth_size; depth++) {
			// Create a mask for this slice
			std::vector<BlockType> mask(plane_size1 * plane_size2, EMPTY);

			// Fill the mask for this slice
			for (int p2 = 0; p2 < plane_size2; p2++) {
				for (int p1 = 0; p1 < plane_size1; p1++) {
					// Convert plane coordinates back to x,y,z
					int coords[3];
					coords[depth_coord] = depth;
					coords[plane_coord1] = p1;
					coords[plane_coord2] = p2;
					int x = coords[0], y = coords[1], z = coords[2];

					if (shouldRenderFace(x, y, z, face)) {
						mask[p1 + p2 * plane_size1] = at(x, y, z);
					}
					else {
						mask[p1 + p2 * plane_size1] = EMPTY;
					}
				}
			}

			// Generate quads from the mask using greedy meshing
			for (int p2 = 0; p2 < plane_size2; p2++) {
				for (int p1 = 0; p1 < plane_size1; ) {
					if (mask[p1 + p2 * plane_size1] != EMPTY) {
						BlockType blockType = mask[p1 + p2 * plane_size1];

						// Compute width (extend in p1 direction)
						int width = 1;
						while (p1 + width < plane_size1 &&
							mask[(p1 + width) + p2 * plane_size1] == blockType) {
							width++;
						}

						// Compute height (extend in p2 direction)
						int height = 1;
						bool canExtendHeight = true;
						while (p2 + height < plane_size2 && canExtendHeight) {
							// Check if the entire row can be extended
							for (int k = 0; k < width; k++) {
								if (mask[(p1 + k) + (p2 + height) * plane_size1] != blockType) {
									canExtendHeight = false;
									break;
								}
							}
							if (canExtendHeight) {
								height++;
							}
						}

						// Create the quad
						Quad quad;

						// Convert plane coordinates back to block coordinates
						int block_coords[3];
						block_coords[depth_coord] = depth;
						block_coords[plane_coord1] = p1;
						block_coords[plane_coord2] = p2;

						quad.x = block_coords[0];
						quad.y = block_coords[1];
						quad.z = block_coords[2];

						
						switch (face) {
						case 0: // +X face
						case 1: // -X face
							quad.width = height; 
							quad.height = width;  
							break;
						case 2: // +Y face
						case 3: // -Y face		
						case 4: // +Z face
						case 5: // -Z face
							quad.width = width;   
							quad.height = height; 
							break;
						}

						quad.face = (Face)face;
						quad.type = blockType;

						quads.push_back(quad);

						// Clear the mask for the area we just processed
						for (int h = 0; h < height; h++) {
							for (int w = 0; w < width; w++) {
								mask[(p1 + w) + (p2 + h) * plane_size1] = EMPTY;
							}
						}

						p1 += width;
					}
					else {
						p1++;
					}
				}
			}
		}
	}

	return quads;
}

std::optional<std::reference_wrapper<BlockType>> Chunks::GetWorldBlock(int x, int y, int z)
{
	int localX = x;
	int localY = y;
	int localZ = z;

	int chunkOffsetX = 0, chunkOffsetY = 0, chunkOffsetZ = 0;

	// Wrap coordinate into the local chunk or figure out neighbor chunk
	if (x < 0) {
		chunkOffsetX = -1;
		localX = m_Width + x;
	}
	else if (x >= m_Width) {
		chunkOffsetX = 1;
		localX = x - m_Width;
	}

	if (y < 0) {
		chunkOffsetY = -1;
		localY = m_Height + y;
	}
	else if (y >= m_Height) {
		chunkOffsetY = 1;
		localY = y - m_Height;
	}

	if (z < 0) {
		chunkOffsetZ = -1;
		localZ = m_Depth + z;
	}
	else if (z >= m_Depth) {
		chunkOffsetZ = 1;
		localZ = z - m_Depth;
	}

	if (chunkOffsetX == 0 && chunkOffsetY == 0 && chunkOffsetZ == 0) {
		return at(localX, localY, localZ);
	}

	if (m_Voxel) return std::nullopt;

	std::shared_ptr<Chunks> neighbor = m_Voxel->GetChunk( m_X + chunkOffsetX, m_Y + chunkOffsetY, m_Z + chunkOffsetZ);

	if (!neighbor) return std::nullopt;
	return std::ref(neighbor->at(localX, localY, localZ));
}

void Chunks::FillChunk(BlockType type)
{
	std::fill(m_Blocks.begin(), m_Blocks.end(), type); // Fill the entire chunk with the specified block type
}
