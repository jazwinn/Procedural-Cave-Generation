#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <map>
#include <vector>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	
	Shader(const char* vertexfile, const char* fragmentfile);


	void Activate();

	void Delete();

    

	GLuint m_ID;

private:
    std::map<std::string, int> uniformLocations; // store uniform

    GLint getUniformLocation(const std::string& name)
    {
        // Search for the uniform location in the cache
        auto pos = uniformLocations.find(name);
        if (pos == uniformLocations.end())
            // If not found, get the location from OpenGL and store it in the cache
            return uniformLocations[name] = glGetUniformLocation(m_ID, name.c_str());
        return pos->second; // Return cached location if found
    }

    void compileErrors(unsigned int shader, const char* type);

public:
    // Set a uniform variable of type vec3 (float x, y, z)
    void setUniform(const std::string& name, float x, float y, float z) {
        glUniform3f(getUniformLocation(name), x, y, z);
    }

    // Set a uniform variable of type vec2 (glm::vec2)
    void setUniform(const std::string& name, const glm::vec2& v) {
        glUniform2f(getUniformLocation(name), v.x, v.y);
    }

    // Set a uniform variable of type vec3 (glm::vec3)
    void setUniform(const std::string& name, const glm::vec3& v) {
        this->setUniform(name, v.x, v.y, v.z);
    }

    // Set a uniform variable of type vec4 (glm::vec4)
    void setUniform(const std::string& name, const glm::vec4& v) {
        glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w);
    }

    // Set a uniform variable for vector types (std::vector<float>)
    void setUniform(const std::string& name, const std::vector<float>& v) {
        if (v.size() == 1)
            glUniform1f(getUniformLocation(name), v[0]);
        else if (v.size() == 2)
            glUniform2f(getUniformLocation(name), v[0], v[1]);
        else if (v.size() == 3)
            glUniform3f(getUniformLocation(name), v[0], v[1], v[2]);
        else if (v.size() == 4)
            glUniform4f(getUniformLocation(name), v[0], v[1], v[2], v[3]);
    }

    // Set a uniform matrix4fv (4x4 matrix, typically used for transformations)
    void setUniform(const std::string& name, const glm::mat4& m) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &m[0][0]);
    }

    // Set a uniform matrix3fv (3x3 matrix)
    void setUniform(const std::string& name, const glm::mat3& m) {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &m[0][0]);
    }

    // Set a uniform float variable
    void setUniform(const std::string& name, float val) {
        glUniform1f(getUniformLocation(name), val);
    }

    // Set a uniform integer variable
    void setUniform(const std::string& name, int val) {
        glUniform1i(getUniformLocation(name), val);
    }

    // Set a uniform boolean variable (converted to int)
    void setUniform(const std::string& name, bool val) {
        glUniform1i(getUniformLocation(name), val);
    }

    // Set a uniform unsigned integer variable
    void setUniform(const std::string& name, GLuint val) {
        glUniform1ui(getUniformLocation(name), val);
    }
};

