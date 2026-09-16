#pragma once

// GLM includes
#include "glm/glm.hpp"

namespace pldn
{
    class Shader
    {
    public:
        Shader();
        ~Shader();

        // Loads the simplest shaders, and automatically binds the program.
        bool LazyTempLoad();

        bool Load(const char* vertexData, const char* fragmentData);
        void Bind() const;
        void Unbind() const;

        void SetInt(const char* name, const int integer);
        void SetVector2(const char* name, const glm::vec2& vector);
        void SetMatrix4(const char* name, const glm::mat4& matrix);
        // void SetVector4(const char* name, const glm::vec4& value) {}
        // void SetInt(const char* name, int value) {}
        
    private:
        unsigned int m_shaderProgramHandle = 0;
    };
}