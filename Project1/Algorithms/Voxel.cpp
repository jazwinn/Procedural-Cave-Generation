#include "Voxel.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>


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
                        quad.x = face_x + m_X;
                        quad.y = face_y + m_Y;
                        quad.z = face_z + m_Z;

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
}

void Voxel::UpdateChunk(int key)
{
	auto it = m_Chunks.find(key);
	if (it != m_Chunks.end()) {
		// Chunk exists, update it
        std::vector<Quad> quads = it->second->GenerateQuads();

		std::vector<glm::mat4x4> transforms;

		float scaleFactor = it->second->GetScale(); // Scale factor for the cubes
		glm::vec3 chunkPosition = glm::vec3(0.f);

		for (const auto& quad : quads) {

			//Chunk transformation
			glm::mat4 model = glm::mat4(1.f);
			model = glm::translate(model, chunkPosition);
			model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		

			glm::vec3 faceOffset;
			switch (quad.face) {
			case 0: faceOffset = glm::vec3(0.5f, 0.0f, 0.0f); break; // +X
			case 1: faceOffset = glm::vec3(-0.5f, 0.0f, 0.0f); break; // -X
			case 2: faceOffset = glm::vec3(0.0f, 0.5f, 0.0f); break; // +Y
			case 3: faceOffset = glm::vec3(0.0f, -0.5f, 0.0f); break; // -Y
			case 4: faceOffset = glm::vec3(0.0f, 0.0f, 0.5f); break; // +Z
			case 5: faceOffset = glm::vec3(0.0f, 0.0f, -0.5f); break; // -Z
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
