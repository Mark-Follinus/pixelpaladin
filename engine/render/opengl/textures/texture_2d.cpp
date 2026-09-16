#include "texture_2d.hpp"

// Standard library includes
#include <cstdio>

// GLFW & GLAD libraries
#include <glad/glad.h>

// STB header
#define STB_IMAGE_IMPLEMENTATION
#include "../external/include/stb/stb_image.h"

namespace pldn
{

// ------------------------
// Public Constructors
// ------------------------

	Texture2D::Texture2D() {}

	Texture2D::~Texture2D() 
	{
        if (m_textureHandle != 0)
        {
            glDeleteTextures(1, &m_textureHandle);
        }
	}
	
// ------------------------
// Public Methods
// ------------------------

	bool Texture2D::Load(const char* path)
	{
        stbi_set_flip_vertically_on_load(true);

        int channels = 0;

        unsigned char* pixels = stbi_load(path, &m_width, &m_height, &channels, STBI_rgb_alpha);

        if (pixels == nullptr)
        {
            std::printf("Error: Failed to load texture: %s\nReason: %s\n", path, stbi_failure_reason());
            return false;
        }

        glGenTextures(1, &m_textureHandle);
        glBindTexture(GL_TEXTURE_2D, m_textureHandle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        stbi_image_free(pixels);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
	}

	void Texture2D::Bind(unsigned int slot) const
	{
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_textureHandle);
	}

// ------------------------
// Private Methods
// ------------------------

}