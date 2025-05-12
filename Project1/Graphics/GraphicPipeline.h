#pragma once

#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#include <vector>

namespace graphic {
	class GraphicPipeline
	{
	public:
		GraphicPipeline();
		~GraphicPipeline();
		
		void Draw();
	
	private:

		std::vector<Shader> vec_Shader;
		std::vector<VAO> vec_VAO;
		std::vector<VBO> vec_VBO;
		std::vector<EBO> vec_EBO;
		std::vector<Texture> vec_Texture;
	};
}


