#include "shader.hpp"

// Standard library includes
#include <cstdio> 

// GLFW & GLAD libraries
#include <glad/glad.h>

// GLM includes
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace pldn
{

// ------------------------
// Public Constructors
// ------------------------

	Shader::Shader() {}

	Shader::~Shader() {}

// ------------------------
// Public Methods
// ------------------------

	bool Shader::LazyTempLoad()
	{
		const char* vertexShaderSource =
			"#version 460 core\n"
			"\n"
			"layout(location = 0) in vec3 aPosition;\n"
			"layout(location = 1) in vec2 aUV;\n"
			"\n"
			"uniform mat4 uProjection;\n"
			"uniform mat4 uView;\n"
			"uniform mat4 uModel;\n"
			"\n"
			"uniform vec2 uUVOffset;\n"
			"uniform vec2 uUVScale;\n"
			"\n"
			"out vec2 vUV;\n"
			"\n"
			"void main()\n"
			"{\n"
			"   vUV = uUVOffset + aUV * uUVScale;\n"
			"	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);\n"
			"}\0";

		const char* fragmentShaderSource =
			"#version 460 core\n"
			"\n"
			"in vec2 vUV;\n"
			"\n"
			"uniform sampler2D uTexture;\n"
			"\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"	FragColor = texture(uTexture, vUV);\n"
			"}\0";

		if (!Load(vertexShaderSource, fragmentShaderSource))
		{
			printf("Error: Shader program failed to create.\n");
			return false;
		}

		Bind();
		return true;
	}

	bool Shader::Load(const char* vertexData, const char* fragmentData)
	{
		GLuint m_vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(m_vertexShaderHandle, 1, &vertexData, NULL);
		glCompileShader(m_vertexShaderHandle);

		int success = 0;
		char infoLog[512] = {};

		glGetShaderiv(m_vertexShaderHandle, GL_COMPILE_STATUS, &success);
		if (success == 0)
		{
			glGetShaderInfoLog(m_vertexShaderHandle, sizeof(infoLog), NULL, infoLog);
			std::printf("Error: Vertex shader failed to compile.\n %s\n", infoLog);
			return false;
		}

		GLuint m_fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(m_fragmentShaderHandle, 1, &fragmentData, NULL);
		glCompileShader(m_fragmentShaderHandle);

		success = 0;

		glGetShaderiv(m_fragmentShaderHandle, GL_COMPILE_STATUS, &success);
		if (success == 0)
		{
			glGetShaderInfoLog(m_fragmentShaderHandle, sizeof(infoLog), NULL, infoLog);
			std::printf("Error: Fragment shader failed to compile.\n %s\n", infoLog);
			return false;
		}

		m_shaderProgramHandle = glCreateProgram();
		glAttachShader(m_shaderProgramHandle, m_vertexShaderHandle);
		glAttachShader(m_shaderProgramHandle, m_fragmentShaderHandle);
		glLinkProgram(m_shaderProgramHandle);

		success = 0;

		glGetProgramiv(m_shaderProgramHandle, GL_LINK_STATUS, &success);
		if (success == 0)
		{
			glGetProgramInfoLog(m_shaderProgramHandle, sizeof(infoLog), nullptr, infoLog);

			std::printf("Error: Shader program failed to link.\n %s\n", infoLog);
			return false;
		}

		glDeleteShader(m_vertexShaderHandle);
		glDeleteShader(m_fragmentShaderHandle);
		return true;
	}

	void Shader::Bind() const
	{
		glUseProgram(m_shaderProgramHandle);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::SetInt(const char* name, const int integer)
	{
		const GLint location = glGetUniformLocation(m_shaderProgramHandle, name);
		glUniform1i(location, integer);
	}

	void Shader::SetVector2(const char* name, const glm::vec2& vector)
	{
		const GLint location = glGetUniformLocation(m_shaderProgramHandle, name);
		glUniform2fv(location, 1, glm::value_ptr(vector));
	}

	void Shader::SetMatrix4(const char* name, const glm::mat4& matrix)
	{
		const GLint location = glGetUniformLocation(m_shaderProgramHandle, name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

// ------------------------ 
// Private Methods
// ------------------------

}