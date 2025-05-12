#include "Shader.h"

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);

}

Shader::Shader(const char* vertexfile, const char* fragmentfile)
{

	std::string vertexcode = get_file_contents(vertexfile);
	std::string fragmentcode = get_file_contents(fragmentfile);

	const char* vertexsource = vertexcode.c_str();
	const char* fragmentsource = fragmentcode.c_str();

	//create reference to vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexsource, NULL);
	//compile vertex shader
	glCompileShader(vertexShader);

	compileErrors(vertexShader, "VERTEX");

	//check if shader is successfull
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	//compile fragment shader
	glCompileShader(fragmentShader);

	compileErrors(fragmentShader, "FRAGMENT");

	//create a program 
	m_ID = glCreateProgram();

	//attach shader to the program
	glAttachShader(m_ID, vertexShader);
	glAttachShader(m_ID, fragmentShader);
	//link program
	glLinkProgram(m_ID);

	compileErrors(m_ID, "PROGRAM");

	//once shader is in the program, you can delete it
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Shader::Activate()
{
	glUseProgram(m_ID);
}

void Shader::Delete()
{
	glDeleteProgram(m_ID);
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}